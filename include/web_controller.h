#ifndef WEB_CONTROLLER_H
#define WEB_CONTROLLER_H

#define WEB_UPDATE_COOLDOWN 50

void web_controller_setup();
void web_controller_loop();

const char WEB_UPDATE_PAGE[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>
    <form action="/api/update" method="post">
      <select name="mode">
        <option>fan</option><option>auto</option><option selected>cool</option><option>dry</option>
      </select>
      <select name="speed">
        <option>auto</option><option selected>low</option><option>medium</option><option>high</option>
      </select>
      <input name="temp" type="number" min="62" max="86" value="72">
      <button type="submit">Apply</button>
    </form>

    <form action="/api/toggle_off" method="post">
      <button type="submit">Off</button>
    </form>

    <form action="/api/toggle_energy_saver" method="post">
      <button type="submit">Energy saver</button>
    </form>

    <form action="/api/sync" method="post">
      <button type="submit">Sync</button>
    </form>

    <script>
      // ask for the key once, then remember it in this browser
      let key = localStorage.getItem('key');
      if (!key) {
        key = prompt('API key');
        localStorage.setItem('key', key);
      }

      document.querySelectorAll('form').forEach((form) => {
        form.addEventListener('submit', async (e) => {
          e.preventDefault();   // stop the browser from leaving the page
          const res = await fetch(form.action, {
            method: 'POST',
            headers: { 'X-Api-Key': key },
            body: new URLSearchParams(new FormData(form))
          });

          if (res.status === 401) {
            localStorage.removeItem('key');   // wrong key, forget it
            alert('Wrong key. Reload the page to try again.');
          } else if (!res.ok) {
            alert(await res.text());          // e.g. "too many requests"
          }
        });
      });
    </script>
  </body>
  </html>
  )rawliteral";

#endif