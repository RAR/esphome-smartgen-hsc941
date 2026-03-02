#include "smartgen_hsc941_web.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include <cstring>

namespace esphome {
namespace smartgen_hsc941_web {

static const char *const TAG = "smartgen_hsc941_web";

// ============================================================
//  Dashboard HTML  –  stored in flash (.rodata), NOT IRAM
// ============================================================
const char SmartgenHSC941Web::DASHBOARD_HTML[] = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>SmartGen HSC941</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
:root{--bg:#1a1a2e;--card:#16213e;--accent:#0f3460;--hi:#e94560;--ok:#2ecc71;--warn:#f39c12;--txt:#eee;--dim:#888}
body{font-family:'Segoe UI',system-ui,sans-serif;background:var(--bg);color:var(--txt);min-height:100vh;padding:8px}
h1{text-align:center;padding:12px 0;font-size:1.3em;color:var(--hi);letter-spacing:1px}
.bar{display:flex;justify-content:center;gap:6px;padding:4px 0 10px;flex-wrap:wrap}
.bar .status{padding:4px 12px;border-radius:12px;font-size:.75em;font-weight:600}
.conn{background:#2ecc7133;color:var(--ok)}.disc{background:#e9456033;color:var(--hi)}
.mode{background:var(--accent);color:var(--txt)}
.section{background:var(--card);border-radius:8px;padding:10px;margin-bottom:8px}
.section h2{font-size:.85em;color:var(--dim);text-transform:uppercase;letter-spacing:1px;border-bottom:1px solid #ffffff15;padding-bottom:4px;margin-bottom:6px}
.grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(150px,1fr));gap:6px}
.item{background:#ffffff08;border-radius:6px;padding:6px 8px;display:flex;flex-direction:column}
.item .lbl{font-size:.65em;color:var(--dim);text-transform:uppercase}
.item .val{font-size:1.1em;font-weight:600;margin-top:2px}
.alarms{display:grid;grid-template-columns:repeat(auto-fill,minmax(200px,1fr));gap:4px}
.alarm{display:flex;align-items:center;gap:6px;padding:4px 8px;border-radius:4px;font-size:.75em;background:#ffffff05}
.alarm .dot{width:8px;height:8px;border-radius:50%;flex-shrink:0}
.dot.on{background:var(--hi);box-shadow:0 0 6px var(--hi)}.dot.off{background:#333}
.dot.ok{background:var(--ok);box-shadow:0 0 6px var(--ok)}
.dot.warn-on{background:var(--warn);box-shadow:0 0 6px var(--warn)}
.controls{display:flex;flex-wrap:wrap;gap:6px;justify-content:center;padding:6px 0}
.btn{border:none;border-radius:6px;padding:10px 20px;font-size:.85em;font-weight:600;cursor:pointer;transition:all .15s}
.btn:active{transform:scale(.96)}
.btn-start{background:#2ecc71;color:#000}.btn-stop{background:#e74c3c;color:#fff}
.btn-auto{background:#3498db;color:#fff}.btn-manual{background:#f39c12;color:#000}
.btn-gen-on{background:#27ae60;color:#fff}.btn-gen-off{background:#c0392b;color:#fff}
.btn-reset{background:#9b59b6;color:#fff}
.btn:disabled{opacity:.5;cursor:not-allowed}
.toast{position:fixed;bottom:16px;left:50%;transform:translateX(-50%);background:#333;color:#fff;padding:8px 16px;border-radius:8px;font-size:.8em;display:none;z-index:99}
.footer{text-align:center;padding:8px;font-size:.65em;color:var(--dim)}
</style>
</head>
<body>
<h1>⚡ SmartGen HSC941 Dashboard</h1>
<div class="bar">
 <span id="connStatus" class="status disc">OFFLINE</span>
 <span id="modeStatus" class="status mode">---</span>
</div>

<div class="section"><h2>Electrical</h2>
<div class="grid" id="elecGrid"></div></div>

<div class="section"><h2>Engine &amp; Environment</h2>
<div class="grid" id="engGrid"></div></div>

<div class="section"><h2>Power</h2>
<div class="grid" id="pwrGrid"></div></div>

<div class="section"><h2>Runtime</h2>
<div class="grid" id="rtGrid"></div></div>

<div class="section"><h2>Alarms &amp; Warnings</h2>
<div class="alarms" id="alarmGrid"></div></div>

<div class="section"><h2>I/O Status</h2>
<div class="alarms" id="ioGrid"></div></div>

<div class="section"><h2>Controls</h2>
<div class="controls">
 <button class="btn btn-start" onclick="cmd(0)">▶ Start</button>
 <button class="btn btn-stop" onclick="cmd(1)">■ Stop</button>
 <button class="btn btn-auto" onclick="cmd(3)">Auto</button>
 <button class="btn btn-manual" onclick="cmd(4)">Manual</button>
 <button class="btn btn-gen-on" onclick="cmd(6)">Gen ON</button>
 <button class="btn btn-gen-off" onclick="cmd(5)">Gen OFF</button>
 <button class="btn btn-reset" onclick="cmd(7)">Fault Reset</button>
</div></div>

<div class="toast" id="toast"></div>
<div class="footer">SmartGen HSC941 Web UI &bull; ESPHome Component</div>

<script>
const S={
 gen_va:{l:'Gen V (A)',u:'V',g:'elec'},gen_vb:{l:'Gen V (B)',u:'V',g:'elec'},gen_vc:{l:'Gen V (C)',u:'V',g:'elec'},
 gen_vab:{l:'V AB',u:'V',g:'elec'},gen_vbc:{l:'V BC',u:'V',g:'elec'},gen_vca:{l:'V CA',u:'V',g:'elec'},
 gen_freq:{l:'Frequency',u:'Hz',g:'elec'},
 ia:{l:'Current A',u:'A',g:'elec'},ib:{l:'Current B',u:'A',g:'elec'},ic:{l:'Current C',u:'A',g:'elec'},
 water_temp:{l:'Water Temp',u:'°C',g:'eng'},oil_press:{l:'Oil Pressure',u:'kPa',g:'eng'},
 rpm:{l:'Engine RPM',u:'RPM',g:'eng'},batt_v:{l:'Battery',u:'V',g:'eng'},charge_v:{l:'Charge V',u:'V',g:'eng'},
 aux1:{l:'Aux Sensor 1',u:'',g:'eng'},
 pa_kw:{l:'Power A',u:'kW',g:'pwr'},pb_kw:{l:'Power B',u:'kW',g:'pwr'},pc_kw:{l:'Power C',u:'kW',g:'pwr'},
 total_kw:{l:'Total Power',u:'kW',g:'pwr'},kvar:{l:'Reactive',u:'kVAR',g:'pwr'},kva:{l:'Apparent',u:'kVA',g:'pwr'},
 pf:{l:'Power Factor',u:'',g:'pwr'},load_pct:{l:'Load',u:'%',g:'pwr'},
 total_hours:{l:'Total Hours',u:'h',g:'rt'},total_min:{l:'Minutes',u:'m',g:'rt'},
 total_starts:{l:'Total Starts',u:'',g:'rt'},total_kwh:{l:'Total Energy',u:'kWh',g:'rt'},
 eng_status:{l:'Engine Status',u:'',g:'rt'},auto_status:{l:'Auto Status',u:'',g:'rt'}
};
const A={
 alarm:{l:'Common Alarm',t:'sd'},warning:{l:'Common Warning',t:'w'},shutdown:{l:'Common Shutdown',t:'sd'},
 estop:{l:'Emergency Stop',t:'sd'},overspeed_sd:{l:'Overspeed SD',t:'sd'},underspeed_sd:{l:'Underspeed SD',t:'sd'},
 loss_speed_sd:{l:'Loss Speed SD',t:'sd'},overfreq_sd:{l:'Over-Freq SD',t:'sd'},underfreq_sd:{l:'Under-Freq SD',t:'sd'},
 overvolt_sd:{l:'Over-Volt SD',t:'sd'},undervolt_sd:{l:'Under-Volt SD',t:'sd'},overcurr_sd:{l:'Over-Current SD',t:'sd'},
 crank_fail_sd:{l:'Crank Fail SD',t:'sd'},hi_temp_sd_in:{l:'High Temp SD',t:'sd'},lo_op_sd_in:{l:'Low O.P. SD',t:'sd'},
 no_gen_sd:{l:'No Gen SD',t:'sd'},ext_sd:{l:'External SD',t:'sd'},lo_fuel_sd_in:{l:'Low Fuel SD',t:'sd'},
 lo_cool_sd_in:{l:'Low Coolant SD',t:'sd'},hi_temp_w_in:{l:'High Temp Warn',t:'w'},lo_op_w_in:{l:'Low O.P. Warn',t:'w'},
 stop_fail_w:{l:'Stop Fail Warn',t:'w'},lo_fuel_w_in:{l:'Low Fuel Warn',t:'w'},charge_fail_w:{l:'Charge Fail Warn',t:'w'},
 batt_uv_w:{l:'Battery UV Warn',t:'w'},batt_ov_w:{l:'Battery OV Warn',t:'w'},ext_w:{l:'External Warn',t:'w'},
 loss_speed_w:{l:'Loss Speed Warn',t:'w'},lo_cool_w_in:{l:'Low Coolant Warn',t:'w'},
 temp_open_w:{l:'Temp Open Warn',t:'w'},op_open_w:{l:'O.P. Open Warn',t:'w'},
 aux_open_w:{l:'Aux Open Warn',t:'w'},aux_w:{l:'Aux Warn',t:'w'},aux_sd:{l:'Aux SD',t:'sd'},
 temp_open_sd:{l:'Temp Open SD',t:'sd'},op_open_sd:{l:'O.P. Open SD',t:'sd'},
 lo_oil_sd_in:{l:'Low Oil SD',t:'sd'},aux_open_sd:{l:'Aux Open SD',t:'sd'},
 hi_temp_sd:{l:'High Temp (calc)',t:'sd'},lo_op_sd:{l:'Low O.P. (calc)',t:'sd'},
 hi_temp_w:{l:'High Temp W (calc)',t:'w'},lo_op_w:{l:'Low O.P. W (calc)',t:'w'}
};
const IO={
 on_load:{l:'Gen On Load',t:'ok'},auto_mode:{l:'Auto Mode',t:'ok'},manual_mode:{l:'Manual Mode',t:'ok'},
 stop_mode:{l:'Stop Mode',t:'ok'},estop_in:{l:'E-Stop Input',t:'sd'},
 start_relay:{l:'Start Relay',t:'ok'},fuel_relay:{l:'Fuel Relay',t:'ok'},
 aux_in1:{l:'Aux In 1',t:'ok'},aux_in2:{l:'Aux In 2',t:'ok'},aux_in3:{l:'Aux In 3',t:'ok'},
 aux_in4:{l:'Aux In 4',t:'ok'},aux_in5:{l:'Aux In 5',t:'ok'},aux_in6:{l:'Aux In 6',t:'ok'},
 aux_out1:{l:'Aux Out 1',t:'ok'},aux_out2:{l:'Aux Out 2',t:'ok'},aux_out3:{l:'Aux Out 3',t:'ok'},aux_out4:{l:'Aux Out 4',t:'ok'}
};

function mkItem(id,lbl){return `<div class="item"><span class="lbl">${lbl}</span><span class="val" id="${id}">--</span></div>`;}
function mkAlarm(id,lbl){return `<div class="alarm"><span class="dot off" id="d_${id}"></span><span>${lbl}</span></div>`;}

function init(){
 const grps={elec:'elecGrid',eng:'engGrid',pwr:'pwrGrid',rt:'rtGrid'};
 const ghtml={elec:'',eng:'',pwr:'',rt:''};
 for(const[k,v]of Object.entries(S)){ghtml[v.g]+=mkItem('s_'+k,v.l);}
 for(const g in grps)document.getElementById(grps[g]).innerHTML=ghtml[g];
 let ah='';for(const[k,v]of Object.entries(A))ah+=mkAlarm(k,v.l);
 document.getElementById('alarmGrid').innerHTML=ah;
 let ih='';for(const[k,v]of Object.entries(IO))ih+=mkAlarm(k,v.l);
 document.getElementById('ioGrid').innerHTML=ih;
}

function fmtVal(v,u){
 if(v===null||v===undefined)return'--';
 let s=typeof v==='number'?v.toFixed(v>=100?0:v>=10?1:2):String(v);
 return u?s+' '+u:s;
}

function update(d){
 if(!d)return;
 const cs=document.getElementById('connStatus');
 if(d.connected){cs.textContent='ONLINE';cs.className='status conn';}
 else{cs.textContent='OFFLINE';cs.className='status disc';}
 const ms=document.getElementById('modeStatus');
 if(d.binary){
  if(d.binary.auto_mode)ms.textContent='AUTO';
  else if(d.binary.manual_mode)ms.textContent='MANUAL';
  else if(d.binary.stop_mode)ms.textContent='STOP';
  else ms.textContent='---';
 }
 if(d.sensors){for(const[k,v]of Object.entries(S)){const el=document.getElementById('s_'+k);if(el)el.textContent=fmtVal(d.sensors[k],v.u);}}
 if(d.binary){
  for(const[k,v]of Object.entries(A)){
   const dot=document.getElementById('d_'+k);if(!dot)continue;
   const on=d.binary[k]===true;
   dot.className='dot '+(on?(v.t==='w'?'warn-on':'on'):'off');
  }
  for(const[k,v]of Object.entries(IO)){
   const dot=document.getElementById('d_'+k);if(!dot)continue;
   const on=d.binary[k]===true;
   dot.className='dot '+(on?(v.t==='sd'?'on':'ok'):'off');
  }
 }
}

function poll(){
 fetch('/api/status').then(r=>r.json()).then(update).catch(()=>{
  document.getElementById('connStatus').textContent='NO LINK';
  document.getElementById('connStatus').className='status disc';
 });
}

function toast(msg,ms){const t=document.getElementById('toast');t.textContent=msg;t.style.display='block';setTimeout(()=>t.style.display='none',ms||2000);}

function cmd(coil){
 fetch('/api/command',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({coil:coil})})
 .then(r=>r.json()).then(d=>{toast(d.ok?'Command sent':'Command failed');setTimeout(poll,500);})
 .catch(()=>toast('Error sending command'));
}

init();poll();setInterval(poll,3000);
</script>
</body>
</html>)rawliteral";

// ============================================================
//  Component lifecycle
// ============================================================

void SmartgenHSC941Web::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SmartGen HSC941 Web UI...");
  this->start_server_();
}

void SmartgenHSC941Web::dump_config() {
  ESP_LOGCONFIG(TAG, "SmartGen HSC941 Web UI:");
  ESP_LOGCONFIG(TAG, "  Port: %u", this->port_);
  if (this->server_) {
    ESP_LOGCONFIG(TAG, "  Server: running");
  } else {
    ESP_LOGCONFIG(TAG, "  Server: FAILED to start");
  }
}

// ============================================================
//  HTTP Server start / stop
// ============================================================

void SmartgenHSC941Web::start_server_() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = this->port_;
  config.ctrl_port = this->port_ + 32768;  // control port offset
  config.stack_size = 8192;
  config.max_uri_handlers = 4;
  config.lru_purge_enable = true;

  esp_err_t err = httpd_start(&this->server_, &config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to start HTTP server on port %u: %s", this->port_, esp_err_to_name(err));
    this->mark_failed();
    return;
  }
  ESP_LOGI(TAG, "HTTP server started on port %u", this->port_);

  // Register URI handlers
  httpd_uri_t root_uri = {
      .uri = "/",
      .method = HTTP_GET,
      .handler = SmartgenHSC941Web::handle_root_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &root_uri);

  httpd_uri_t status_uri = {
      .uri = "/api/status",
      .method = HTTP_GET,
      .handler = SmartgenHSC941Web::handle_api_status_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &status_uri);

  httpd_uri_t command_uri = {
      .uri = "/api/command",
      .method = HTTP_POST,
      .handler = SmartgenHSC941Web::handle_api_command_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &command_uri);
}

void SmartgenHSC941Web::stop_server_() {
  if (this->server_) {
    httpd_stop(this->server_);
    this->server_ = nullptr;
  }
}

// ============================================================
//  Request handlers
// ============================================================

esp_err_t SmartgenHSC941Web::handle_root_(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  return httpd_resp_send(req, DASHBOARD_HTML, strlen(DASHBOARD_HTML));
}

esp_err_t SmartgenHSC941Web::handle_api_status_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self || !self->controller_) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Controller not available");
    return ESP_FAIL;
  }

  // Build JSON using std::string – with SPIRAM_USE_MALLOC enabled on ESP32-S3
  // the internal buffer is allocated from PSRAM for large allocations, conserving IRAM/DRAM
  std::string json;
  json.reserve(6144);
  self->controller_->build_status_json(json);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json.c_str(), json.size());
}

esp_err_t SmartgenHSC941Web::handle_api_command_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self || !self->controller_) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Controller not available");
    return ESP_FAIL;
  }

  // Read POST body
  int content_len = req->content_len;
  if (content_len <= 0 || content_len > 256) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid request body");
    return ESP_FAIL;
  }

  char body[257];
  int received = httpd_req_recv(req, body, content_len);
  if (received <= 0) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Failed to read body");
    return ESP_FAIL;
  }
  body[received] = '\0';

  // Simple JSON parse for {"coil": N}
  // Find "coil" key and parse the integer value
  const char *coil_key = strstr(body, "\"coil\"");
  if (!coil_key) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing 'coil' field");
    return ESP_FAIL;
  }
  const char *colon = strchr(coil_key, ':');
  if (!colon) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
    return ESP_FAIL;
  }
  int coil = atoi(colon + 1);

  // Validate coil address: only allow known control coils
  bool valid = (coil == 0 || coil == 1 || coil == 3 || coil == 4 ||
                coil == 5 || coil == 6 || coil == 7);
  if (!valid) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid coil address");
    return ESP_FAIL;
  }

  bool ok = self->controller_->write_coil(static_cast<uint16_t>(coil), true);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  const char *resp = ok ? R"({"ok":true})" : R"({"ok":false,"error":"write failed"})";
  return httpd_resp_send(req, resp, strlen(resp));
}

}  // namespace smartgen_hsc941_web
}  // namespace esphome
