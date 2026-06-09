#pragma once

const char* htmlPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Tableau de Bord Robot</title>
  <style>
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #eef2f5; padding: 20px; display: flex; flex-direction: column; align-items: center; gap: 20px; margin: 0; }
    .card { background: white; padding: 20px; border-radius: 12px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); width: 100%; max-width: 850px; box-sizing: border-box; }
    .motor-row { display: flex; align-items: center; gap: 20px; flex-wrap: wrap; justify-content: center; }
    .motor-label { font-size: 24px; font-weight: bold; border: 3px solid #333; border-radius: 10px; width: 75px; height: 75px; display: flex; align-items: center; justify-content: center; color: #333; flex-shrink: 0; text-align: center; }
    .mode-col { display: flex; flex-direction: column; gap: 5px; }
    .state-btn { padding: 8px 15px; font-size: 13px; font-weight: bold; border: 2px solid #ccc; background-color: white; color: #555; border-radius: 6px; cursor: pointer; transition: 0.2s; }
    .state-btn.active { background-color: #28a745; color: white; border-color: #28a745; }
    .slider-col { display: flex; flex-direction: column; align-items: center; gap: 8px; min-width: 160px; flex-grow: 1; }
    .slider-labels { display: flex; justify-content: space-between; width: 100%; font-weight: bold; font-size: 12px; color: #333; }
    input[type=range] { width: 100%; }
    .input-zone { display: flex; align-items: center; gap: 10px; font-size: 14px; font-weight: bold; color: #555; }
    .tours-input { width: 90px; padding: 5px; border: 2px solid #ccc; border-radius: 5px; text-align: center; font-weight: bold; }
    .action-col { display: flex; flex-direction: column; align-items: center; gap: 10px; }
    .rpm-box { border: 3px solid #333; border-radius: 10px; padding: 5px 15px; text-align: center; font-size: 13px; font-weight: bold; color: #333; min-width: 90px; }
    .rpm-value { font-size: 18px; color: #007bff; display: block; margin-top: 2px; }
    .action-buttons { display: flex; gap: 10px; width: 100%; }
    .send-btn, .stop-btn { flex: 1; padding: 8px; font-size: 12px; font-weight: bold; color: white; border: none; border-radius: 6px; cursor: pointer; }
    .send-btn { background-color: #333; }
    .stop-btn { background-color: #dc3545; }
    .stop-all-btn { width: 100%; padding: 15px; font-size: 24px; font-weight: bold; background-color: #dc3545; color: white; border: none; border-radius: 12px; cursor: pointer; box-shadow: 0 6px 15px rgba(220,53,69,0.4); }
    #status { font-weight: bold; color: #007bff; text-align: center; margin: 0; }
    .config-zone { display: flex; justify-content: center; align-items: center; gap: 15px; background: #fff3cd; padding: 10px; border-radius: 8px; color: #856404; font-weight: bold; margin-bottom: 10px; }
    .config-zone input { padding: 5px; width: 80px; text-align: center; font-weight: bold; border: 1px solid #ffeeba; border-radius: 4px; }
    .config-zone button { background: #ffc107; color: #212529; border: none; padding: 6px 12px; font-weight: bold; border-radius: 4px; cursor: pointer; }
    .gamepad-zone { display: none; background: #d4edda; color: #155724; border: 1px solid #c3e6cb; flex-direction: column; align-items: center; gap: 10px; }
    .gamepad-slider-group { display: flex; align-items: center; gap: 15px; width: 100%; justify-content: space-between; }
    .gamepad-zone input[type=range] { width: 150px; accent-color: #28a745; }
  </style>
</head>
<body>
  <h2>Tableau de Bord du Robot</h2>
  <div style="display: flex; gap: 10px; flex-wrap: wrap; justify-content: center; width: 100%; max-width: 850px;">
    <div class="card config-zone" style="flex: 1; min-width: 280px; margin: 0;">
      <label>Ticks / Tour :</label>
      <input type="number" id="ticksInput" value="340.0" step="1">
      <button onclick="updateTicks()">Appliquer</button>
    </div>
    <div class="card config-zone gamepad-zone" id="gamepad-ui" style="flex: 1; min-width: 280px; margin: 0;">
      <div class="gamepad-slider-group">
        <label>🎮 Limite Vitesse : <span id="val-gamepad-max">50</span>%</label>
        <input type="range" id="gamepad-max" min="5" max="100" value="50" oninput="document.getElementById('val-gamepad-max').innerText = this.value">
      </div>
      <div class="gamepad-slider-group">
        <label>🔄 Sensibilité Virage : <span id="val-gamepad-sens">60</span>%</label>
        <input type="range" id="gamepad-sens" min="10" max="100" value="60" oninput="document.getElementById('val-gamepad-sens').innerText = this.value">
      </div>
    </div>
  </div>
  <div class="card motor-row" style="border: 2px solid #007bff; margin-top: 10px;">
    <div class="motor-label" style="background:#007bff; color:white; border-color:#0056b3;">ROBOT<br><small>(Tous)</small></div>
    <div class="mode-col">
      <button id="btn-m0-FWD" class="state-btn active" onclick="setMode(0, 'FWD')">FWD</button>
      <button id="btn-m0-BWD" class="state-btn" onclick="setMode(0, 'BWD')">BWD</button>
      <button id="btn-m0-TOURS" class="state-btn" onclick="setMode(0, 'TOURS')">TOURS</button>
    </div>
    <div class="slider-col">
      <div class="slider-labels"><span>0</span><span>VIT: <span id="val-vit-m0">50</span>%</span><span>100</span></div>
      <input type="range" id="vit-m0" min="0" max="100" value="50" oninput="updateVit(0)">
      <div class="input-zone">
        <span id="wrapper-chk-m0"><input type="checkbox" id="chk-m0" onchange="renderInputs(0)"> Infini</span>
        <span id="input-container-m0"><input type="number" id="val-m0" class="tours-input" placeholder="nb. Sec." step="0.5" value="3.0"></span>
      </div>
    </div>
    <div class="action-col">
      <div class="action-buttons" style="flex-direction: column;">
        <button class="send-btn" onclick="envoyer(0)" style="width:100px;">ENVOYER</button>
        <button class="stop-btn" onclick="stopMoteur(0)" style="width:100px;">STOP</button>
      </div>
    </div>
  </div>
  <div class="card motor-row">
    <div class="motor-label">M1<br><small>(Gauche)</small></div>
    <div class="mode-col">
      <button id="btn-m1-FWD" class="state-btn active" onclick="setMode(1, 'FWD')">FWD</button>
      <button id="btn-m1-BWD" class="state-btn" onclick="setMode(1, 'BWD')">BWD</button>
      <button id="btn-m1-TOURS" class="state-btn" onclick="setMode(1, 'TOURS')">TOURS</button>
    </div>
    <div class="slider-col">
      <div class="slider-labels"><span>0</span><span>VIT: <span id="val-vit-m1">50</span>%</span><span>100</span></div>
      <input type="range" id="vit-m1" min="0" max="100" value="50" oninput="updateVit(1)">
      <div class="input-zone">
        <span id="wrapper-chk-m1"><input type="checkbox" id="chk-m1" onchange="renderInputs(1)"> Infini</span>
        <span id="input-container-m1"><input type="number" id="val-m1" class="tours-input" placeholder="nb. Sec." step="0.5" value="3.0"></span>
      </div>
    </div>
    <div class="action-col">
      <div class="rpm-box">RPM M1<span class="rpm-value" id="rpm-m1">0.0</span></div>
      <div class="action-buttons"><button class="send-btn" onclick="envoyer(1)">ENVOYER</button><button class="stop-btn" onclick="stopMoteur(1)">STOP</button></div>
    </div>
  </div>
  <div class="card motor-row">
    <div class="motor-label">M2<br><small>(Droit)</small></div>
    <div class="mode-col">
      <button id="btn-m2-FWD" class="state-btn active" onclick="setMode(2, 'FWD')">FWD</button>
      <button id="btn-m2-BWD" class="state-btn" onclick="setMode(2, 'BWD')">BWD</button>
      <button id="btn-m2-TOURS" class="state-btn" onclick="setMode(2, 'TOURS')">TOURS</button>
    </div>
    <div class="slider-col">
      <div class="slider-labels"><span>0</span><span>VIT: <span id="val-vit-m2">50</span>%</span><span>100</span></div>
      <input type="range" id="vit-m2" min="0" max="100" value="50" oninput="updateVit(2)">
      <div class="input-zone">
        <span id="wrapper-chk-m2"><input type="checkbox" id="chk-m2" onchange="renderInputs(2)"> Infini</span>
        <span id="input-container-m2"><input type="number" id="val-m2" class="tours-input" placeholder="nb. Sec." step="0.5" value="3.0"></span>
      </div>
    </div>
    <div class="action-col">
      <div class="rpm-box">RPM M2<span class="rpm-value" id="rpm-m2">0.0</span></div>
      <div class="action-buttons"><button class="send-btn" onclick="envoyer(2)">ENVOYER</button><button class="stop-btn" onclick="stopMoteur(2)">STOP</button></div>
    </div>
  </div>
  <div class="card" style="padding: 10px; width: 100%; max-width: 850px;">
    <button class="stop-all-btn" onclick="stopAll()">⚠️ STOP ALL ⚠️</button>
    <p id="status" style="margin-top: 10px;">Prêt.</p>
  </div>
  <script>
    let modes = { 1: 'FWD', 2: 'FWD', 0: 'FWD' };
    function updateVit(id) { document.getElementById('val-vit-m' + id).innerText = document.getElementById('vit-m' + id).value; }
    function setMode(id, mode) {
      modes[id] = mode;
      document.getElementById('btn-m'+id+'-FWD').classList.remove('active');
      document.getElementById('btn-m'+id+'-BWD').classList.remove('active');
      document.getElementById('btn-m'+id+'-TOURS').classList.remove('active');
      document.getElementById('btn-m'+id+'-'+mode).classList.add('active');
      renderInputs(id);
    }
    function renderInputs(id) {
      let mode = modes[id];
      let isInfini = document.getElementById('chk-m'+id).checked;
      let container = document.getElementById('input-container-m'+id);
      if (mode === 'TOURS') {
        document.getElementById('wrapper-chk-m'+id).style.display = 'none';
        container.innerHTML = '<input type="number" id="val-m'+id+'" class="tours-input" placeholder="nb. Tours" step="0.1" value="1.0">';
      } else {
        document.getElementById('wrapper-chk-m'+id).style.display = 'inline-block';
        if (isInfini) { container.innerHTML = '<span style="color: #28a745; font-size:18px;">∞</span>'; } 
        else { container.innerHTML = '<input type="number" id="val-m'+id+'" class="tours-input" placeholder="nb. Sec." step="0.5" value="3.0">'; }
      }
    }
    function envoyer(id) {
      let mode = modes[id]; let vit = document.getElementById('vit-m' + id).value; let cmd = "";
      if (mode === 'TOURS') { let tours = document.getElementById('val-m'+id).value || "1"; cmd = id + " A " + vit + " tour " + tours; } 
      else {
        let isInfini = document.getElementById('chk-m'+id).checked; let dir = (mode === 'FWD') ? " A " : " R ";
        if (isInfini) { cmd = id + dir + vit; } else { let sec = document.getElementById('val-m'+id).value || "3"; cmd = id + dir + vit + " sec " + sec; }
      }
      executerRequete(cmd);
    }
    function stopMoteur(id) { if(id === 0) stopAll(); else executerRequete(id + " A 0"); }
    function stopAll() { fetch('/cmd?c=1 A 0').then(() => setTimeout(() => fetch('/cmd?c=2 A 0'), 100)); document.getElementById("status").innerText = "ARRÊT GÉNÉRAL"; }
    function updateTicks() { fetch('/tune?val=' + encodeURIComponent(document.getElementById('ticksInput').value)); }
    function executerRequete(cmd) { fetch('/cmd?c=' + encodeURIComponent(cmd)).then(() => document.getElementById("status").innerText = "Commande : " + cmd); }
    setInterval(function() { fetch('/status').then(r => r.json()).then(data => { document.getElementById('rpm-m1').innerText = data.rpm1; document.getElementById('rpm-m2').innerText = data.rpm2; }); }, 150);

    let gamepadIndex = null; let lastCmdM1 = ""; let lastCmdM2 = "";
    window.addEventListener("gamepadconnected", (e) => { gamepadIndex = e.gamepad.index; document.getElementById("status").innerHTML = "🎮 <b>Manette détectée :</b> " + e.gamepad.id; document.getElementById("gamepad-ui").style.display = "flex"; });
    window.addEventListener("gamepaddisconnected", (e) => { gamepadIndex = null; document.getElementById("gamepad-ui").style.display = "none"; stopAll(); });
    setInterval(() => {
      if (gamepadIndex !== null) {
        let gp = navigator.getGamepads()[gamepadIndex];
        if (gp) {
          let y = -gp.axes[1]; let x = -gp.axes[2];
          if (Math.abs(y) < 0.15) y = 0; if (Math.abs(x) < 0.15) x = 0;
          let sensVirage = parseInt(document.getElementById('gamepad-sens').value) / 100.0; x = x * sensVirage;
          let speedLeft = y + x; let speedRight = y - x;
          let maxAxis = Math.max(Math.abs(speedLeft), Math.abs(speedRight)); if (maxAxis > 1.0) { speedLeft /= maxAxis; speedRight /= maxAxis; }
          let maxLimit = parseInt(document.getElementById('gamepad-max').value);
          let valM1 = Math.round(speedLeft * maxLimit); let valM2 = Math.round(speedRight * maxLimit);
          let dirM1 = valM1 >= 0 ? "A" : "R"; let dirM2 = valM2 >= 0 ? "A" : "R";
          let cmdM1 = "1 " + dirM1 + " " + Math.abs(valM1); let cmdM2 = "2 " + dirM2 + " " + Math.abs(valM2);
          if (cmdM1 !== lastCmdM1 || cmdM2 !== lastCmdM2) {
            if (valM1 === 0 && valM2 === 0) { fetch('/cmd?c=1 A 0').then(() => fetch('/cmd?c=2 A 0')); } 
            else { fetch('/cmd?c=' + encodeURIComponent(cmdM1)).then(() => fetch('/cmd?c=' + encodeURIComponent(cmdM2))); }
            lastCmdM1 = cmdM1; lastCmdM2 = cmdM2;
          }
        }
      }
    }, 100);
  </script>
</body>
</html>
)rawliteral";