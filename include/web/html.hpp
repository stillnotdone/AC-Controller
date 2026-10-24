#ifndef WEB_HTML_H
#define WEB_HTML_H

#include <Arduino.h>

const char WEB_UPDATER_HTML[] PROGMEM = R"html(<!DOCTYPE html>
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>
  :root {
    color-scheme: light dark;
  }

  body {
    font-family: system-ui, sans-serif;
    max-width: 480px;
    margin: 0 auto;
    padding: 16px;
    -webkit-tap-highlight-color: transparent;
  }

  #status {
    text-align: center;
    margin: 8px 0 28px;
  }

  #temp {
    font-size: 4.5em;
    font-weight: bold;
    line-height: 1;
  }

  #details {
    font-size: 1.1em;
    opacity: .7;
    margin-top: 8px;
    text-transform: capitalize;
  }

  form {
    display: flex;
    flex-wrap: wrap;
    gap: 10px;
    margin-bottom: 14px;
  }

  select, input, button {
    font-size: 1em;
    padding: 16px;
    border: 1px solid #8888;
    border-radius: 12px;
    flex: 1 1 40%;
    min-width: 0;
    touch-action: manipulation;
  }

  input:disabled {
    opacity: .4;
  }

  button {
    background: #2f6fed;
    color: #fff;
    border: none;
    font-weight: bold;
    cursor: pointer;
  }

  button:active {
    opacity: .7;
  }

  button.on {
    background: #2a9d4a;
  }

  button.off {
    background: #d33;
  }

  [action$=sync] button {
    background: #777;
  }
  #toast {
    position: fixed;
    left: 50%;
    bottom: 24px;
    transform: translate(-50%, 20px);
    background: #2a9d4a;
    color: #fff;
    font-weight: bold;
    padding: 12px 24px;
    border-radius: 999px;
    opacity: 0;
    transition: opacity .2s, transform .2s;
    pointer-events: none;
  }

  #toast.show {
    opacity: 1;
    transform: translate(-50%, 0);
  }
</style>
<div id="status">
  <div id="temp">--</div>
  <div id="details">Loading...</div>
</div>
<div id="toast">Sent</div>
<form id="update" action="/api/update" method="post">
  <select name="mode">
    <option>fan</option>
    <option>auto</option>
    <option>cool</option>
    <option>dry</option>
  </select>
  <select name="speed">
    <option>auto</option>
    <option>low</option>
    <option>medium</option>
    <option>high</option>
  </select>
  <input name="temp" type="number" min="62" max="86">
  <button>Apply</button>
</form>

<form action="/api/disable" method="post">
  <button id="power">Off</button>
</form>

<form action="/api/toggle_energy_saver" method="post">
  <button id="saver">Energy saver</button>
</form>

<form action="/api/sync" method="post">
  <button>Sync</button>
</form>

<script>
  const updateForm = document.getElementById('update');
  let enabled = true;
  let key = localStorage.getItem('key');
  if (!key) {
    key = prompt('API key');
    localStorage.setItem('key', key);
  }

  async function api(url, options = {}, quiet = false) {
    options.headers = { 'X-Api-Key': key };
    const res = await fetch(url, options);
    if (res.status === 401) {
      localStorage.removeItem('key');
      alert('Wrong key. Reload the page to try again.');
      return;
    }
    if (!res.ok) {
      if (!quiet) alert(await res.text());
      return;
    }
    return res;
  }

  function updateTempLock() {
    updateForm.temp.disabled = updateForm.mode.value === 'fan';
  }

  let firstLoad = true;

  async function loadStatus() {
    const res = await api('/api/status', {}, true);
    if (!res) {
      document.getElementById('details').textContent = 'Offline';
      return;
    }
    const s = await res.json();
    enabled = s.enabled;

    if (firstLoad) {
      firstLoad = false;
      updateForm.mode.value = s.mode;
      updateForm.speed.value = s.speed;
      updateForm.temp.value = s.temp;
      updateTempLock();
    }

    document.getElementById('temp').textContent = s.temp + '\u00B0F';
    document.getElementById('details').textContent = s.mode + ' \u00B7 ' + s.speed;

    const power = document.getElementById('power');
    power.textContent = s.enabled ? 'On' : 'Off';
    power.className = s.enabled ? 'on' : 'off';

    document.getElementById('saver').className = s.energy_saver ? 'on' : 'off';
  }
  
  let toastTimer;

  function showToast() {
    const toast = document.getElementById('toast');
    toast.classList.add('show');
    clearTimeout(toastTimer);
    toastTimer = setTimeout(() => toast.classList.remove('show'), 1500);
  }

  updateForm.mode.onchange = updateTempLock;
  for (const form of document.forms) {
    form.onsubmit = async (e) => {
      e.preventDefault();
      let url = form.action;
      let data = new FormData(form);

      if (form.action.endsWith('/api/disable') && !enabled) {
        url = '/api/update';
        data = new FormData(updateForm);
      }

      const res = await api(url, {
        method: 'POST',
        body: new URLSearchParams(data)
      });
      if (res) showToast();
    };
  }

  const timer = setInterval(loadStatus, 500);
  loadStatus();
</script>
)html";

#endif