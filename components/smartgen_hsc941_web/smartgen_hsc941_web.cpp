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
<title>HSC941 Genset Controller</title>
<style>
*,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
:root{
 --bg:#0c0e14;--surface:#141720;--card:#1a1e2b;--border:#252a3a;
 --text:#dee2ec;--dim:#6b7394;--faint:#3a4060;
 --red:#ef4444;--orange:#f59e0b;--green:#22c55e;--blue:#3b82f6;--cyan:#06b6d4;--purple:#a855f7;
 --red-bg:#ef444418;--orange-bg:#f59e0b18;--green-bg:#22c55e18;--blue-bg:#3b82f618;
 --gauge-track:#1e2235;--radius:10px;
}
html{font-size:14px}
body{font-family:'Inter',system-ui,-apple-system,sans-serif;background:var(--bg);color:var(--text);min-height:100vh;overflow-x:hidden}
@media(max-width:768px){html{font-size:13px}}

/* ── Header ── */
.hdr{background:var(--surface);border-bottom:1px solid var(--border);padding:12px 20px;display:flex;align-items:center;justify-content:space-between;position:sticky;top:0;z-index:50;backdrop-filter:blur(12px)}
.hdr-left{display:flex;align-items:center;gap:12px}
.hdr-logo{width:36px;height:36px;background:linear-gradient(135deg,var(--blue),var(--cyan));border-radius:8px;display:flex;align-items:center;justify-content:center}
.hdr-logo svg{width:20px;height:20px;fill:#fff}
.hdr-title{font-size:1.1rem;font-weight:700;letter-spacing:-.02em}
.hdr-sub{font-size:.7rem;color:var(--dim);font-weight:500;letter-spacing:.04em;text-transform:uppercase}
.hdr-right{display:flex;align-items:center;gap:10px}
.badge{padding:4px 10px;border-radius:20px;font-size:.68rem;font-weight:700;letter-spacing:.04em;text-transform:uppercase;white-space:nowrap}
.badge-conn{background:var(--green-bg);color:var(--green);border:1px solid #22c55e30}
.badge-disc{background:var(--red-bg);color:var(--red);border:1px solid #ef444430;animation:pulse-red 2s infinite}
.badge-mode{background:var(--blue-bg);color:var(--blue);border:1px solid #3b82f630}
.badge-auto{background:var(--green-bg);color:var(--green);border:1px solid #22c55e30}
.badge-manual{background:#f59e0b18;color:var(--orange);border:1px solid #f59e0b30}
.badge-stop{background:var(--red-bg);color:var(--red);border:1px solid #ef444430}
@keyframes pulse-red{0%,100%{opacity:1}50%{opacity:.6}}

/* ── Layout ── */
.main{max-width:1400px;margin:0 auto;padding:16px 20px 80px}
.row{display:grid;gap:12px;margin-bottom:12px}
.r-gauges{grid-template-columns:repeat(4,1fr)}
.r-dual{grid-template-columns:1fr 1fr}
.r-triple{grid-template-columns:1fr 1fr 1fr}
.r-full{grid-template-columns:1fr}
@media(max-width:1024px){.r-gauges{grid-template-columns:repeat(2,1fr)}.r-triple{grid-template-columns:1fr}}
@media(max-width:600px){.r-gauges{grid-template-columns:1fr 1fr}.r-dual{grid-template-columns:1fr}}

/* ── Card ── */
.card{background:var(--card);border:1px solid var(--border);border-radius:var(--radius);overflow:hidden}
.card-hd{padding:10px 14px;border-bottom:1px solid var(--border);display:flex;align-items:center;gap:8px}
.card-hd .ico{width:16px;height:16px;opacity:.5}
.card-hd h2{font-size:.72rem;font-weight:600;text-transform:uppercase;letter-spacing:.06em;color:var(--dim)}
.card-body{padding:14px}

/* ── Gauge (SVG ring) ── */
.gauge-wrap{display:flex;flex-direction:column;align-items:center;padding:16px 8px}
.gauge-svg{width:140px;height:140px}
.gauge-track{fill:none;stroke:var(--gauge-track);stroke-width:10;stroke-linecap:round}
.gauge-fill{fill:none;stroke-width:10;stroke-linecap:round;transition:stroke-dashoffset .8s cubic-bezier(.4,0,.2,1),stroke .5s}
.gauge-center{font-size:1.5rem;font-weight:700;fill:var(--text);text-anchor:middle;dominant-baseline:central}
.gauge-unit{font-size:.6rem;fill:var(--dim);text-anchor:middle;font-weight:500}
.gauge-label{margin-top:6px;font-size:.72rem;font-weight:600;color:var(--dim);text-transform:uppercase;letter-spacing:.05em}
.gauge-range{font-size:.6rem;color:var(--faint);margin-top:2px}

/* ── Data Table ── */
.dtable{width:100%;border-collapse:collapse}
.dtable tr{border-bottom:1px solid #ffffff06}
.dtable tr:last-child{border-bottom:none}
.dtable td{padding:7px 0;vertical-align:middle}
.dtable .dlbl{color:var(--dim);font-size:.75rem;font-weight:500;width:50%}
.dtable .dval{text-align:right;font-size:.85rem;font-weight:600;font-variant-numeric:tabular-nums;white-space:nowrap}
.dtable .dunit{color:var(--dim);font-size:.7rem;font-weight:400;margin-left:3px}

/* ── Phase bars (3-phase visual) ── */
.phases{display:grid;grid-template-columns:1fr 1fr 1fr;gap:8px;margin-top:10px}
.phase{background:var(--surface);border-radius:8px;padding:10px;text-align:center}
.phase-label{font-size:.65rem;font-weight:700;color:var(--dim);text-transform:uppercase;letter-spacing:.06em;margin-bottom:6px}
.phase-val{font-size:1.25rem;font-weight:700;font-variant-numeric:tabular-nums}
.phase-sub{font-size:.65rem;color:var(--dim);margin-top:3px;font-weight:500}
.ph-a .phase-val{color:#f87171}.ph-b .phase-val{color:#fbbf24}.ph-c .phase-val{color:#60a5fa}
.ph-a{border-top:2px solid #f87171}.ph-b{border-top:2px solid #fbbf24}.ph-c{border-top:2px solid #60a5fa}

/* ── Annunciator Panel ── */
.ann-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(175px,1fr));gap:2px}
.ann{display:flex;align-items:center;gap:8px;padding:6px 10px;border-radius:6px;font-size:.72rem;font-weight:500;transition:background .3s}
.ann.active-sd{background:#ef444420;color:#fca5a5}.ann.active-w{background:#f59e0b20;color:#fcd34d}
.ann .lamp{width:10px;height:10px;border-radius:50%;flex-shrink:0;border:1.5px solid #ffffff10;transition:all .3s}
.lamp.off{background:#252a3a}
.lamp.red{background:var(--red);box-shadow:0 0 8px var(--red),0 0 20px #ef444440}
.lamp.amber{background:var(--orange);box-shadow:0 0 8px var(--orange),0 0 20px #f59e0b40}
.lamp.green{background:var(--green);box-shadow:0 0 8px var(--green),0 0 20px #22c55e40}
.lamp.blue{background:var(--blue);box-shadow:0 0 6px var(--blue)}

/* ── I/O row ── */
.io-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(180px,1fr));gap:6px}
.io-item{display:flex;align-items:center;gap:8px;padding:8px 12px;border-radius:8px;font-size:.74rem;font-weight:500;color:var(--dim);background:var(--surface);border:1px solid transparent;transition:all .3s}
.io-item .io-lbl{flex:1}
.io-item .io-state{font-size:.6rem;font-weight:700;letter-spacing:.06em;text-transform:uppercase;padding:2px 8px;border-radius:4px;flex-shrink:0}
.io-item.off-state .io-state{background:#ffffff08;color:var(--faint)}
.io-item.on-green{color:var(--text);background:#22c55e0d;border-color:#22c55e25}
.io-item.on-green .io-state{background:#22c55e28;color:var(--green)}
.io-item.on-blue{color:var(--text);background:#3b82f60d;border-color:#3b82f625}
.io-item.on-blue .io-state{background:#3b82f628;color:var(--blue)}
.io-item.on-red{color:var(--text);background:#ef44440d;border-color:#ef444425}
.io-item.on-red .io-state{background:#ef444428;color:var(--red)}
.io-item.on-amber{color:var(--text);background:#f59e0b0d;border-color:#f59e0b25}
.io-item.on-amber .io-state{background:#f59e0b28;color:var(--orange)}

/* ── Controls ── */
.ctrl-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(130px,1fr));gap:8px}
.ctrl-btn{position:relative;border:none;border-radius:8px;padding:14px 10px;font-size:.78rem;font-weight:700;cursor:pointer;transition:all .15s;text-transform:uppercase;letter-spacing:.04em;color:#fff;overflow:hidden}
.ctrl-btn::before{content:'';position:absolute;inset:0;opacity:0;transition:opacity .15s;background:rgba(255,255,255,.08)}
.ctrl-btn:hover::before{opacity:1}
.ctrl-btn:active{transform:scale(.97)}
.ctrl-btn:disabled{opacity:.35;cursor:not-allowed;transform:none}
.btn-start{background:linear-gradient(135deg,#16a34a,#22c55e)}.btn-stop{background:linear-gradient(135deg,#dc2626,#ef4444)}
.btn-auto{background:linear-gradient(135deg,#2563eb,#3b82f6)}.btn-manual{background:linear-gradient(135deg,#d97706,#f59e0b);color:#000}
.btn-gen-on{background:linear-gradient(135deg,#0d9488,#14b8a6)}.btn-gen-off{background:linear-gradient(135deg,#9f1239,#e11d48)}
.btn-reset{background:linear-gradient(135deg,#7c3aed,#a855f7)}
.ctrl-btn .btn-ico{font-size:1.2rem;display:block;margin-bottom:4px}

/* ── Confirmation modal ── */
.modal-overlay{position:fixed;inset:0;background:rgba(0,0,0,.65);backdrop-filter:blur(4px);display:none;z-index:100;align-items:center;justify-content:center}
.modal-overlay.show{display:flex}
.modal{background:var(--card);border:1px solid var(--border);border-radius:12px;padding:24px;max-width:360px;width:90%;text-align:center}
.modal h3{font-size:1rem;margin-bottom:6px}.modal p{font-size:.8rem;color:var(--dim);margin-bottom:16px}
.modal-btns{display:flex;gap:8px;justify-content:center}
.modal-btns button{padding:8px 24px;border-radius:6px;font-size:.78rem;font-weight:600;cursor:pointer;border:none;transition:all .15s}
.modal-btns .m-cancel{background:var(--surface);color:var(--text);border:1px solid var(--border)}
.modal-btns .m-confirm{background:var(--blue);color:#fff}
.modal-btns .m-danger{background:var(--red);color:#fff}

/* ── Toast ── */
.toast{position:fixed;bottom:20px;left:50%;transform:translateX(-50%) translateY(80px);background:var(--card);border:1px solid var(--border);color:var(--text);padding:10px 20px;border-radius:8px;font-size:.78rem;font-weight:600;z-index:200;transition:transform .3s cubic-bezier(.4,0,.2,1);pointer-events:none}
.toast.show{transform:translateX(-50%) translateY(0)}
.toast.ok{border-color:#22c55e40}.toast.err{border-color:#ef444440}

/* ── Runtime bar ── */
.runtime-row{display:flex;gap:16px;flex-wrap:wrap}
.runtime-item{flex:1;min-width:100px;text-align:center;padding:10px;background:var(--surface);border-radius:8px}
.runtime-val{font-size:1.3rem;font-weight:700;font-variant-numeric:tabular-nums}
.runtime-lbl{font-size:.6rem;color:var(--dim);text-transform:uppercase;letter-spacing:.06em;margin-top:2px;font-weight:600}

/* ── Scrollbar ── */
::-webkit-scrollbar{width:6px}::-webkit-scrollbar-track{background:var(--bg)}::-webkit-scrollbar-thumb{background:var(--faint);border-radius:3px}
</style>
</head>
<body>

<!-- Header -->
<header class="hdr">
 <div class="hdr-left">
  <div class="hdr-logo"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg></div>
  <div>
   <div class="hdr-title">HSC941 Genset Controller</div>
   <div class="hdr-sub" id="hdrTime">SmartGen &bull; Modbus RTU</div>
  </div>
 </div>
 <div class="hdr-right">
  <span class="badge badge-disc" id="connBadge">OFFLINE</span>
  <span class="badge badge-mode" id="modeBadge">---</span>
 </div>
</header>

<main class="main">

<!-- Gauges Row -->
<div class="row r-gauges">
 <div class="card"><div class="card-body gauge-wrap" id="gVolt"></div></div>
 <div class="card"><div class="card-body gauge-wrap" id="gFreq"></div></div>
 <div class="card"><div class="card-body gauge-wrap" id="gRPM"></div></div>
 <div class="card"><div class="card-body gauge-wrap" id="gLoad"></div></div>
</div>

<!-- 3-Phase Electrical + Engine -->
<div class="row r-dual">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg><h2>Generator Output</h2></div>
  <div class="card-body">
   <div class="phases">
    <div class="phase ph-a"><div class="phase-label">Phase A</div><div class="phase-val" id="pv_a">--</div><div class="phase-sub"><span id="pi_a">--</span> A &bull; <span id="pp_a">--</span> kW</div></div>
    <div class="phase ph-b"><div class="phase-label">Phase B</div><div class="phase-val" id="pv_b">--</div><div class="phase-sub"><span id="pi_b">--</span> A &bull; <span id="pp_b">--</span> kW</div></div>
    <div class="phase ph-c"><div class="phase-label">Phase C</div><div class="phase-val" id="pv_c">--</div><div class="phase-sub"><span id="pi_c">--</span> A &bull; <span id="pp_c">--</span> kW</div></div>
   </div>
   <table class="dtable" style="margin-top:10px">
    <tr><td class="dlbl">Line-Line AB</td><td class="dval"><span id="v_ab">--</span><span class="dunit">V</span></td></tr>
    <tr><td class="dlbl">Line-Line BC</td><td class="dval"><span id="v_bc">--</span><span class="dunit">V</span></td></tr>
    <tr><td class="dlbl">Line-Line CA</td><td class="dval"><span id="v_ca">--</span><span class="dunit">V</span></td></tr>
    <tr><td class="dlbl">Total Active</td><td class="dval"><span id="v_tkw">--</span><span class="dunit">kW</span></td></tr>
    <tr><td class="dlbl">Reactive</td><td class="dval"><span id="v_kvar">--</span><span class="dunit">kVAR</span></td></tr>
    <tr><td class="dlbl">Apparent</td><td class="dval"><span id="v_kva">--</span><span class="dunit">kVA</span></td></tr>
    <tr><td class="dlbl">Power Factor</td><td class="dval"><span id="v_pf">--</span></td></tr>
   </table>
  </div>
 </div>
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="3"/><path d="M12 1v6m0 6v6m-7.07-3.07l4.24-4.24m5.66-5.66l4.24-4.24M1 12h6m6 0h6m-3.07 7.07l-4.24-4.24m-5.66-5.66L4.93 4.93"/></svg><h2>Engine</h2></div>
  <div class="card-body">
   <table class="dtable">
    <tr><td class="dlbl">Water Temperature</td><td class="dval"><span id="v_wt">--</span><span class="dunit">&#176;C</span></td></tr>
    <tr><td class="dlbl">Oil Pressure</td><td class="dval"><span id="v_op">--</span><span class="dunit">kPa</span></td></tr>
    <tr><td class="dlbl">Battery Voltage</td><td class="dval"><span id="v_bv">--</span><span class="dunit">V</span></td></tr>
    <tr><td class="dlbl">Charge Voltage</td><td class="dval"><span id="v_cv">--</span><span class="dunit">V</span></td></tr>
    <tr><td class="dlbl">Engine Speed</td><td class="dval"><span id="v_rpm2">--</span><span class="dunit">RPM</span></td></tr>
    <tr><td class="dlbl">Aux Sensor 1</td><td class="dval"><span id="v_aux">--</span></td></tr>
    <tr><td class="dlbl">Engine State</td><td class="dval"><span id="v_es">--</span></td></tr>
    <tr><td class="dlbl">Auto State</td><td class="dval"><span id="v_as">--</span></td></tr>
   </table>
  </div>
 </div>
</div>

<!-- Runtime -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><path d="M12 6v6l4 2"/></svg><h2>Runtime &amp; Totals</h2></div>
  <div class="card-body">
   <div class="runtime-row">
    <div class="runtime-item"><div class="runtime-val" id="rt_hrs">--</div><div class="runtime-lbl">Total Hours</div></div>
    <div class="runtime-item"><div class="runtime-val" id="rt_starts">--</div><div class="runtime-lbl">Total Starts</div></div>
    <div class="runtime-item"><div class="runtime-val" id="rt_kwh">--</div><div class="runtime-lbl">Total kWh</div></div>
    <div class="runtime-item"><div class="runtime-val" id="rt_fw">--</div><div class="runtime-lbl">Firmware</div></div>
    <div class="runtime-item"><div class="runtime-val" id="rt_hw">--</div><div class="runtime-lbl">Hardware</div></div>
   </div>
  </div>
 </div>
</div>

<!-- Annunciator -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10.29 3.86L1.82 18a2 2 0 001.71 3h16.94a2 2 0 001.71-3L13.71 3.86a2 2 0 00-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg><h2>Alarm Annunciator</h2></div>
  <div class="card-body"><div class="ann-grid" id="annPanel"></div></div>
 </div>
</div>

<!-- I/O -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="2" y="3" width="20" height="18" rx="2"/><path d="M8 7h8m-8 4h8m-8 4h5"/></svg><h2>Inputs / Outputs</h2></div>
  <div class="card-body"><div class="io-grid" id="ioPanel"></div></div>
 </div>
</div>

<!-- Controls -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 00.33 1.82l.06.06a2 2 0 01-2.83 2.83l-.06-.06a1.65 1.65 0 00-1.82-.33 1.65 1.65 0 00-1 1.51V21a2 2 0 01-4 0v-.09A1.65 1.65 0 009 19.4a1.65 1.65 0 00-1.82.33l-.06.06a2 2 0 01-2.83-2.83l.06-.06A1.65 1.65 0 004.68 15a1.65 1.65 0 00-1.51-1H3a2 2 0 010-4h.09A1.65 1.65 0 004.6 9a1.65 1.65 0 00-.33-1.82l-.06-.06a2 2 0 012.83-2.83l.06.06A1.65 1.65 0 009 4.68a1.65 1.65 0 001-1.51V3a2 2 0 014 0v.09a1.65 1.65 0 001 1.51 1.65 1.65 0 001.82-.33l.06-.06a2 2 0 012.83 2.83l-.06.06A1.65 1.65 0 0019.4 9a1.65 1.65 0 001.51 1H21a2 2 0 010 4h-.09a1.65 1.65 0 00-1.51 1z"/></svg><h2>Controls</h2></div>
  <div class="card-body">
   <div class="ctrl-grid">
    <button class="ctrl-btn btn-start" onclick="confirmCmd(0,'Start Engine','Send START command to the generator?')"><span class="btn-ico">&#9654;</span>Start</button>
    <button class="ctrl-btn btn-stop" onclick="confirmCmd(1,'Stop Engine','Send STOP command to the generator?','danger')"><span class="btn-ico">&#9632;</span>Stop</button>
    <button class="ctrl-btn btn-auto" onclick="confirmCmd(3,'Auto Mode','Switch to AUTO mode?')"><span class="btn-ico">&#8635;</span>Auto</button>
    <button class="ctrl-btn btn-manual" onclick="confirmCmd(4,'Manual Mode','Switch to MANUAL mode?')"><span class="btn-ico">&#9881;</span>Manual</button>
    <button class="ctrl-btn btn-gen-on" onclick="confirmCmd(6,'Close Transfer Switch','Transfer load to generator?')"><span class="btn-ico">&#9889;</span>Xfer Close</button>
    <button class="ctrl-btn btn-gen-off" onclick="confirmCmd(5,'Open Transfer Switch','Transfer load to mains?','danger')"><span class="btn-ico">&#9675;</span>Xfer Open</button>
    <button class="ctrl-btn btn-reset" onclick="confirmCmd(7,'Fault Reset','Reset all active faults?')"><span class="btn-ico">&#8634;</span>Reset</button>
   </div>
  </div>
 </div>
</div>
</main>

<!-- Confirm Modal -->
<div class="modal-overlay" id="confirmModal">
 <div class="modal">
  <h3 id="modalTitle">Confirm</h3>
  <p id="modalMsg">Are you sure?</p>
  <div class="modal-btns">
   <button class="m-cancel" onclick="closeModal()">Cancel</button>
   <button class="m-confirm" id="modalOk" onclick="execCmd()">Confirm</button>
  </div>
 </div>
</div>

<!-- Toast -->
<div class="toast" id="toast"></div>

<script>
/* ── SVG Gauge factory ── */
function mkGauge(containerId,label,unit,min,max,color,decimals){
 const R=58,C=2*Math.PI*R,arc=0.75,len=C*arc;
 const rot=360*(1-arc)/2+90;
 const el=document.getElementById(containerId);
 el.innerHTML=`
  <svg class="gauge-svg" viewBox="0 0 140 140">
   <circle class="gauge-track" cx="70" cy="70" r="${R}" stroke-dasharray="${len} ${C}" transform="rotate(${rot} 70 70)"/>
   <circle class="gauge-fill" cx="70" cy="70" r="${R}" stroke="${color}" stroke-dasharray="0 ${C}" transform="rotate(${rot} 70 70)" id="gf_${containerId}"/>
   <text class="gauge-center" x="70" y="66" id="gv_${containerId}">--</text>
   <text class="gauge-unit" x="70" y="84">${unit}</text>
  </svg>
  <div class="gauge-label">${label}</div>
  <div class="gauge-range">${min} &ndash; ${max} ${unit}</div>`;
 return{
  set(v){
   const pct=v===null?0:Math.max(0,Math.min(1,(v-min)/(max-min)));
   const o=len-(pct*len);
   const fill=document.getElementById('gf_'+containerId);
   const txt=document.getElementById('gv_'+containerId);
   if(fill){fill.style.strokeDasharray=len+' '+C;fill.style.strokeDashoffset=o;}
   if(txt)txt.textContent=v===null?'--':v.toFixed(decimals||0);
  }
 };
}

const G={};
function initGauges(){
 G.volt=mkGauge('gVolt','Voltage','V',0,500,'#60a5fa',0);
 G.freq=mkGauge('gFreq','Frequency','Hz',0,65,'#a78bfa',1);
 G.rpm=mkGauge('gRPM','Engine RPM','RPM',0,2000,'#f59e0b',0);
 G.load=mkGauge('gLoad','Load','%',0,120,'#22c55e',1);
}

/* ── Annunciator definitions ── */
const ANN=[
 {k:'alarm',l:'Common Alarm',t:'sd'},{k:'warning',l:'Common Warning',t:'w'},{k:'shutdown',l:'Common Shutdown',t:'sd'},
 {k:'estop',l:'Emergency Stop',t:'sd'},{k:'overspeed_sd',l:'Overspeed',t:'sd'},{k:'underspeed_sd',l:'Underspeed',t:'sd'},
 {k:'loss_speed_sd',l:'Speed Signal Loss',t:'sd'},{k:'overfreq_sd',l:'Over-Frequency',t:'sd'},{k:'underfreq_sd',l:'Under-Frequency',t:'sd'},
 {k:'overvolt_sd',l:'Over-Voltage',t:'sd'},{k:'undervolt_sd',l:'Under-Voltage',t:'sd'},{k:'overcurr_sd',l:'Over-Current',t:'sd'},
 {k:'crank_fail_sd',l:'Crank Failure',t:'sd'},{k:'hi_temp_sd_in',l:'High Temp SD (input)',t:'sd'},{k:'lo_op_sd_in',l:'Low Oil Press SD (input)',t:'sd'},
 {k:'hi_temp_sd',l:'High Temp SD (calc)',t:'sd'},{k:'lo_op_sd',l:'Low Oil Press SD (calc)',t:'sd'},
 {k:'no_gen_sd',l:'No Generator',t:'sd'},{k:'ext_sd',l:'External Shutdown',t:'sd'},
 {k:'lo_fuel_sd_in',l:'Low Fuel SD',t:'sd'},{k:'lo_cool_sd_in',l:'Low Coolant SD',t:'sd'},
 {k:'lo_oil_sd_in',l:'Low Oil Level SD',t:'sd'},{k:'temp_open_sd',l:'Temp Sensor Open SD',t:'sd'},
 {k:'op_open_sd',l:'O.P. Sensor Open SD',t:'sd'},{k:'aux_open_sd',l:'Aux Sensor Open SD',t:'sd'},{k:'aux_sd',l:'Aux Sensor SD',t:'sd'},
 {k:'hi_temp_w_in',l:'High Temp Warning (input)',t:'w'},{k:'lo_op_w_in',l:'Low Oil Press Warning (input)',t:'w'},
 {k:'hi_temp_w',l:'High Temp Warning (calc)',t:'w'},{k:'lo_op_w',l:'Low Oil Press Warning (calc)',t:'w'},
 {k:'stop_fail_w',l:'Stop Failure',t:'w'},{k:'lo_fuel_w_in',l:'Low Fuel Warning',t:'w'},
 {k:'charge_fail_w',l:'Charge Failure',t:'w'},{k:'batt_uv_w',l:'Battery Under-Voltage',t:'w'},{k:'batt_ov_w',l:'Battery Over-Voltage',t:'w'},
 {k:'ext_w',l:'External Warning',t:'w'},{k:'loss_speed_w',l:'Speed Signal Loss Warning',t:'w'},
 {k:'lo_cool_w_in',l:'Low Coolant Warning',t:'w'},{k:'temp_open_w',l:'Temp Sensor Open',t:'w'},
 {k:'op_open_w',l:'O.P. Sensor Open',t:'w'},{k:'aux_open_w',l:'Aux Sensor Open',t:'w'},{k:'aux_w',l:'Aux Sensor Warning',t:'w'}
];
const IOS=[
 {k:'on_load',l:'Gen On Load (Xfer Closed)',c:'green'},{k:'auto_mode',l:'Auto Mode',c:'blue'},{k:'manual_mode',l:'Manual Mode',c:'amber'},
 {k:'stop_mode',l:'Stop Mode',c:'red'},{k:'estop_in',l:'E-Stop Input',c:'red'},
 {k:'start_relay',l:'Start Relay',c:'green'},{k:'fuel_relay',l:'Fuel Relay',c:'green'},
 {k:'aux_in1',l:'Aux Input 1',c:'blue'},{k:'aux_in2',l:'Aux Input 2',c:'blue'},{k:'aux_in3',l:'Aux Input 3',c:'blue'},
 {k:'aux_in4',l:'Aux Input 4',c:'blue'},{k:'aux_in5',l:'Aux Input 5',c:'blue'},{k:'aux_in6',l:'Aux Input 6',c:'blue'},
 {k:'aux_out1',l:'Aux Output 1',c:'green'},{k:'aux_out2',l:'Aux Output 2',c:'green'},
 {k:'aux_out3',l:'Aux Output 3',c:'green'},{k:'aux_out4',l:'Aux Output 4',c:'green'}
];

function initPanels(){
 let ah='';
 ANN.forEach(a=>{ah+=`<div class="ann" id="ann_${a.k}"><span class="lamp off" id="lmp_${a.k}"></span><span>${a.l}</span></div>`;});
 document.getElementById('annPanel').innerHTML=ah;
 let ih='';
 IOS.forEach(io=>{ih+=`<div class="io-item off-state" id="io_${io.k}"><span class="lamp off" id="iol_${io.k}"></span><span class="io-lbl">${io.l}</span><span class="io-state">OFF</span></div>`;});
 document.getElementById('ioPanel').innerHTML=ih;
}

/* ── Formatters ── */
function f(v,d){if(v===null||v===undefined)return'--';return typeof v==='number'?v.toFixed(d!==undefined?d:(v>=100?0:v>=10?1:2)):String(v);}

/* ── State update ── */
function update(d){
 if(!d)return;
 // Connection
 const cb=document.getElementById('connBadge');
 if(d.connected){cb.textContent='ONLINE';cb.className='badge badge-conn';}
 else{cb.textContent='OFFLINE';cb.className='badge badge-disc';}
 // Mode
 const mb=document.getElementById('modeBadge');const b=d.binary||{};
 if(b.auto_mode){mb.textContent='AUTO';mb.className='badge badge-auto';}
 else if(b.manual_mode){mb.textContent='MANUAL';mb.className='badge badge-manual';}
 else if(b.stop_mode){mb.textContent='STOP';mb.className='badge badge-stop';}
 else{mb.textContent='---';mb.className='badge badge-mode';}
 const s=d.sensors||{};
 // Gauges: prefer gen_va, fallback to first available
 const vavg=(s.gen_va!==null&&s.gen_va!==undefined)?s.gen_va:null;
 G.volt.set(vavg);G.freq.set(s.gen_freq!=null?s.gen_freq:null);
 G.rpm.set(s.rpm!=null?s.rpm:null);G.load.set(s.load_pct!=null?s.load_pct:null);
 // Phase cards
 const ids=[['pv_a','gen_va'],['pv_b','gen_vb'],['pv_c','gen_vc'],['pi_a','ia'],['pi_b','ib'],['pi_c','ic'],['pp_a','pa_kw'],['pp_b','pb_kw'],['pp_c','pc_kw']];
 ids.forEach(([el,k])=>{const e=document.getElementById(el);if(e)e.textContent=f(s[k]);});
 // Line-line + power
 const t=[['v_ab','gen_vab'],['v_bc','gen_vbc'],['v_ca','gen_vca'],['v_tkw','total_kw'],['v_kvar','kvar'],['v_kva','kva'],['v_pf','pf']];
 t.forEach(([el,k])=>{const e=document.getElementById(el);if(e)e.textContent=f(s[k]);});
 // Engine
 const eng=[['v_wt','water_temp'],['v_op','oil_press'],['v_bv','batt_v'],['v_cv','charge_v'],['v_rpm2','rpm'],['v_aux','aux1'],['v_es','eng_status'],['v_as','auto_status']];
 eng.forEach(([el,k])=>{const e=document.getElementById(el);if(e)e.textContent=f(s[k]);});
 // Runtime
 let hours=s.total_hours!=null?Math.floor(s.total_hours):0;
 let mins=s.total_min!=null?Math.floor(s.total_min):0;
 document.getElementById('rt_hrs').textContent=hours.toLocaleString()+'h '+String(mins).padStart(2,'0')+'m';
 document.getElementById('rt_starts').textContent=s.total_starts!=null?Math.floor(s.total_starts).toLocaleString():'--';
 document.getElementById('rt_kwh').textContent=s.total_kwh!=null?Math.floor(s.total_kwh).toLocaleString():'--';
 document.getElementById('rt_fw').textContent=s.sw_ver!=null?'v'+f(s.sw_ver,1):'--';
 document.getElementById('rt_hw').textContent=s.hw_ver!=null?'v'+f(s.hw_ver,1):'--';
 // Header sub with controller time
 if(s.ctrl_hour!=null){
  const ts=String(Math.floor(s.ctrl_hour)).padStart(2,'0')+':'+String(Math.floor(s.ctrl_min||0)).padStart(2,'0')+':'+String(Math.floor(s.ctrl_sec||0)).padStart(2,'0');
  document.getElementById('hdrTime').textContent='Controller '+ts;
 }
 // Annunciator
 ANN.forEach(a=>{
  const on=b[a.k]===true;
  const lamp=document.getElementById('lmp_'+a.k);
  const row=document.getElementById('ann_'+a.k);
  if(lamp)lamp.className='lamp '+(on?(a.t==='w'?'amber':'red'):'off');
  if(row)row.className='ann'+(on?(a.t==='w'?' active-w':' active-sd'):'');
 });
 // I/O
 IOS.forEach(io=>{
  const on=b[io.k]===true;
  const lamp=document.getElementById('iol_'+io.k);
  const row=document.getElementById('io_'+io.k);
  if(lamp)lamp.className='lamp '+(on?io.c:'off');
  if(row)row.className='io-item '+(on?'on-'+io.c:'off-state');
  const st=row?row.querySelector('.io-state'):null;
  if(st)st.textContent=on?'ON':'OFF';
 });
}

/* ── Polling ── */
let fails=0;
function poll(){
 fetch('/api/status').then(r=>r.json()).then(d=>{fails=0;update(d);}).catch(()=>{
  fails++;
  const cb=document.getElementById('connBadge');
  cb.textContent=fails>2?'NO LINK':'...';cb.className='badge badge-disc';
 });
}

/* ── Command with confirmation ── */
let pendingCoil=null;
function confirmCmd(coil,title,msg,style){
 pendingCoil=coil;
 document.getElementById('modalTitle').textContent=title;
 document.getElementById('modalMsg').textContent=msg;
 const ok=document.getElementById('modalOk');
 ok.className=style==='danger'?'m-danger':'m-confirm';
 ok.textContent='Confirm';
 document.getElementById('confirmModal').classList.add('show');
}
function closeModal(){document.getElementById('confirmModal').classList.remove('show');pendingCoil=null;}
function execCmd(){
 if(pendingCoil===null)return;
 const coil=pendingCoil;closeModal();
 fetch('/api/command',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({coil:coil})})
 .then(r=>r.json()).then(d=>{toast(d.ok?'Command sent successfully':'Command failed','ok');setTimeout(poll,500);})
 .catch(()=>toast('Communication error','err'));
}
function toast(msg,cls){
 const t=document.getElementById('toast');t.textContent=msg;t.className='toast show '+(cls||'');
 setTimeout(()=>t.classList.remove('show'),2500);
}

/* ── Init ── */
initGauges();initPanels();poll();setInterval(poll,2500);
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
