#include "smartgen_hsc941_web.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include <cstring>
#include <ctime>
#include <sys/stat.h>

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
html{font-size:16px}
body{font-family:'Inter',system-ui,-apple-system,sans-serif;background:var(--bg);color:var(--text);min-height:100vh;overflow-x:hidden}
@media(max-width:768px){html{font-size:15px}}

/* ── Header ── */
.hdr{background:var(--surface);border-bottom:1px solid var(--border);padding:12px 20px;display:flex;align-items:center;justify-content:space-between;position:sticky;top:0;z-index:50;backdrop-filter:blur(12px)}
.hdr-left{display:flex;align-items:center;gap:12px}
.hdr-logo{width:36px;height:36px;background:linear-gradient(135deg,var(--blue),var(--cyan));border-radius:8px;display:flex;align-items:center;justify-content:center}
.hdr-logo svg{width:20px;height:20px;fill:#fff}
.hdr-title{font-size:1.1rem;font-weight:700;letter-spacing:-.02em}
.hdr-sub{font-size:.7rem;color:var(--dim);font-weight:500;letter-spacing:.04em;text-transform:uppercase}
.hdr-right{display:flex;align-items:center;gap:10px}
.badge-ok{background:#16a34a;color:#fff}
.badge-warn{background:#d97706;color:#fff;animation:pulse-warn 1.5s infinite}
.badge-sd{background:#dc2626;color:#fff;animation:pulse-sd 1s infinite}
@keyframes pulse-warn{0%,100%{opacity:1}50%{opacity:.6}}
@keyframes pulse-sd{0%,100%{opacity:1}50%{opacity:.5}}
/* ── Nav Tabs ── */
.nav{display:flex;gap:0;background:var(--surface);border-bottom:1px solid var(--border);padding:0 16px;position:sticky;top:52px;z-index:49}
.nav-tab{padding:10px 18px;font-size:.78rem;font-weight:600;letter-spacing:.03em;text-transform:uppercase;color:var(--dim);cursor:pointer;border-bottom:2px solid transparent;transition:color .2s,border-color .2s;user-select:none;background:none;border-top:none;border-left:none;border-right:none}
.nav-tab:hover{color:var(--text)}
.nav-tab.active{color:var(--blue);border-bottom-color:var(--blue)}
.section{display:none}
.section.active{display:block}
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
.gauge-center{font-size:1.8rem;font-weight:700;fill:var(--text);text-anchor:middle;dominant-baseline:central}
.gauge-unit{font-size:.75rem;fill:var(--dim);text-anchor:middle;font-weight:500}
.gauge-label{margin-top:6px;font-size:.85rem;font-weight:600;color:var(--dim);text-transform:uppercase;letter-spacing:.05em}
.gauge-range{font-size:.7rem;color:var(--faint);margin-top:2px}

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
.ann-summary{display:flex;flex-direction:column;gap:6px}
.ann-active-list{display:flex;flex-direction:column;gap:4px}
.ann-active{display:flex;align-items:center;gap:10px;padding:8px 12px;border-radius:6px;font-size:.78rem;font-weight:600;animation:ann-pulse 2s ease-in-out infinite}
.ann-active.sd{background:#ef444418;border:1px solid #ef444435;color:#fca5a5}
.ann-active.w{background:#f59e0b18;border:1px solid #f59e0b35;color:#fcd34d}
@keyframes ann-pulse{0%,100%{opacity:1}50%{opacity:.7}}
.ann-active .ann-icon{width:18px;height:18px;flex-shrink:0}
.ann-ok{display:flex;align-items:center;gap:10px;padding:16px;border-radius:8px;background:#22c55e08;border:1px solid #22c55e20;color:var(--green);font-size:.82rem;font-weight:600}
.ann-ok svg{width:20px;height:20px;flex-shrink:0}
.ann-toggle{display:flex;align-items:center;gap:6px;margin-top:8px;padding:0;background:none;border:none;color:var(--dim);font-size:.65rem;cursor:pointer;letter-spacing:.04em;text-transform:uppercase;font-weight:600;transition:color .2s}
.ann-toggle:hover{color:var(--accent)}
.ann-toggle svg{width:12px;height:12px;transition:transform .2s}
.ann-toggle.open svg{transform:rotate(180deg)}
.ann-full{display:none;margin-top:8px}
.ann-full.show{display:block}
.ann-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(175px,1fr));gap:2px}
.ann{display:flex;align-items:center;gap:8px;padding:5px 10px;border-radius:4px;font-size:.68rem;font-weight:500;color:var(--faint);transition:all .3s}
.ann.active-sd{background:#ef444415;color:#fca5a5}.ann.active-w{background:#f59e0b15;color:#fcd34d}
.ann .lamp{width:8px;height:8px;border-radius:50%;flex-shrink:0;transition:all .3s}
.lamp.off{background:#252a3a}
.lamp.red{background:var(--red);box-shadow:0 0 6px var(--red)}
.lamp.amber{background:var(--orange);box-shadow:0 0 6px var(--orange)}
.lamp.green{background:var(--green);box-shadow:0 0 6px var(--green)}
.lamp.blue{background:var(--blue);box-shadow:0 0 4px var(--blue)}

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

/* ── Event log ── */
.evtlog-wrap{max-height:420px;overflow-y:auto;border:1px solid var(--border);border-radius:6px}
.evtlog-tbl{width:100%;border-collapse:collapse;font-size:.72rem}
.evtlog-tbl th{position:sticky;top:0;background:var(--surface);text-align:left;padding:6px 10px;font-size:.62rem;text-transform:uppercase;letter-spacing:.06em;color:var(--dim);border-bottom:1px solid var(--border);font-weight:600}
.evtlog-tbl td{padding:5px 10px;border-bottom:1px solid var(--border);vertical-align:top}
.evtlog-tbl tr:last-child td{border-bottom:none}
.evtlog-tbl .evt-ts{white-space:nowrap;color:var(--dim);width:145px;font-variant-numeric:tabular-nums}
.evtlog-tbl .evt-msg{color:var(--text)}
.evtlog-empty{text-align:center;padding:30px;color:var(--dim);font-size:.8rem}
.evtlog-bar{display:flex;justify-content:space-between;align-items:center;margin-bottom:10px}
.evtlog-count{font-size:.7rem;color:var(--dim)}
.evtlog-clear{font-size:.65rem;padding:4px 12px;background:transparent;border:1px solid var(--border);color:var(--dim);border-radius:4px;cursor:pointer;transition:all .2s}
.evtlog-clear:hover{border-color:var(--accent);color:var(--accent)}

/* ── Scrollbar ── */
::-webkit-scrollbar{width:6px}::-webkit-scrollbar-track{background:var(--bg)}::-webkit-scrollbar-thumb{background:var(--faint);border-radius:3px}

/* ── Relay toggle switch ── */
.relay-item{display:flex;align-items:center;gap:10px;padding:10px 14px;border-radius:8px;background:var(--surface);border:1px solid transparent;transition:all .3s}
.relay-item.relay-on{background:#22c55e0d;border-color:#22c55e25}
.relay-item .relay-lbl{flex:1;font-size:.74rem;font-weight:500;color:var(--dim)}
.relay-item.relay-on .relay-lbl{color:var(--text)}
.toggle{position:relative;width:40px;height:22px;flex-shrink:0;cursor:pointer}
.toggle input{opacity:0;width:0;height:0}
.toggle .slider{position:absolute;inset:0;background:var(--faint);border-radius:11px;transition:all .2s}
.toggle .slider::before{content:'';position:absolute;width:16px;height:16px;left:3px;bottom:3px;background:#fff;border-radius:50%;transition:all .2s}
.toggle input:checked+.slider{background:var(--green)}
.toggle input:checked+.slider::before{transform:translateX(18px)}
.relay-state{font-size:.6rem;font-weight:700;letter-spacing:.06em;text-transform:uppercase;padding:2px 8px;border-radius:4px;flex-shrink:0;min-width:28px;text-align:center}
.relay-item .relay-state{background:#ffffff08;color:var(--faint)}
.relay-item.relay-on .relay-state{background:#22c55e28;color:var(--green)}

/* ── Exercise Schedule ── */
.ex-form{display:grid;grid-template-columns:1fr 1fr;gap:10px}
@media(max-width:600px){.ex-form{grid-template-columns:1fr}}
.ex-field{display:flex;flex-direction:column;gap:4px}
.ex-field label{font-size:.68rem;font-weight:600;text-transform:uppercase;letter-spacing:.05em;color:var(--dim)}
.ex-field select,.ex-field input[type=number]{background:var(--surface);border:1px solid var(--border);border-radius:6px;color:var(--text);padding:8px 10px;font-size:.82rem;font-family:inherit;outline:none;transition:border .2s}
.ex-field select:focus,.ex-field input[type=number]:focus{border-color:var(--blue)}
.ex-field select option{background:var(--card);color:var(--text)}
.ex-row{display:flex;align-items:center;gap:12px;grid-column:1/-1}
.ex-toggle{display:flex;align-items:center;gap:10px}
.ex-toggle span{font-size:.82rem;font-weight:600}
.ex-btns{display:flex;gap:8px;grid-column:1/-1;margin-top:4px}
.ex-btn{padding:10px 20px;border-radius:6px;font-size:.78rem;font-weight:700;cursor:pointer;border:none;transition:all .15s;text-transform:uppercase;letter-spacing:.04em}
.ex-btn:hover{filter:brightness(1.1)}
.ex-btn-save{background:var(--blue);color:#fff}
.ex-btn-run{background:var(--green);color:#fff}
.ex-btn-stop{background:var(--red);color:#fff}
.ex-status{grid-column:1/-1;padding:10px;background:var(--surface);border-radius:8px;font-size:.78rem}
.ex-status .ex-st-label{color:var(--dim);font-size:.68rem;font-weight:600;text-transform:uppercase;letter-spacing:.05em}
.ex-status .ex-st-value{font-weight:700;margin-left:6px}

/* ── Relay Thermostat ── */
.th-relay{background:var(--surface);border-radius:8px;padding:12px;margin-bottom:10px;border:1px solid var(--border)}
.th-relay.th-active{border-color:var(--blue)}
.th-hdr{display:flex;align-items:center;justify-content:space-between;margin-bottom:8px}
.th-name{font-weight:700;font-size:.85rem;display:flex;align-items:center;gap:10px}
.th-reading{font-size:.78rem;padding:3px 8px;border-radius:4px;background:var(--card)}
.th-reading.th-on{color:var(--green);font-weight:700}
.th-reading.th-off{color:var(--dim)}
.th-fields{display:grid;grid-template-columns:1fr 1fr 1fr;gap:8px;align-items:end}
@media(max-width:600px){.th-fields{grid-template-columns:1fr}}
.th-field{display:flex;flex-direction:column;gap:3px}
.th-field label{font-size:.65rem;font-weight:600;text-transform:uppercase;letter-spacing:.05em;color:var(--dim)}
.th-field select,.th-field input[type=number]{background:var(--card);border:1px solid var(--border);border-radius:6px;color:var(--text);padding:7px 9px;font-size:.8rem;font-family:inherit;outline:none;transition:border .2s}
.th-field select:focus,.th-field input[type=number]:focus{border-color:var(--blue)}
.th-field select option{background:var(--card);color:var(--text)}
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
  <span class="badge badge-ok" id="alarmBadge">OK</span>
  <span class="badge badge-disc" id="connBadge">OFFLINE</span>
  <span class="badge badge-mode" id="modeBadge">---</span>
 </div>
</header>

<nav class="nav">
 <button class="nav-tab active" data-sec="monitor" onclick="switchSec('monitor')">Monitoring</button>
 <button class="nav-tab" data-sec="config" onclick="switchSec('config')">Configuration</button>
 <button class="nav-tab" data-sec="history" onclick="switchSec('history')">History</button>
</nav>

<main class="main">

<div class="section active" id="sec-monitor">
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
    <div class="phase ph-a" id="phA"><div class="phase-label">Phase A</div><div class="phase-val" id="pv_a">--</div><div class="phase-sub"><span id="pi_a">--</span> A &bull; <span id="pp_a">--</span> kW</div></div>
    <div class="phase ph-b" id="phB"><div class="phase-label">Phase B</div><div class="phase-val" id="pv_b">--</div><div class="phase-sub"><span id="pi_b">--</span> A &bull; <span id="pp_b">--</span> kW</div></div>
    <div class="phase ph-c" id="phC"><div class="phase-label">Phase C</div><div class="phase-val" id="pv_c">--</div><div class="phase-sub"><span id="pi_c">--</span> A &bull; <span id="pp_c">--</span> kW</div></div>
   </div>
   <table class="dtable" style="margin-top:10px">
    <tr id="llAB"><td class="dlbl">Line-Line AB</td><td class="dval"><span id="v_ab">--</span><span class="dunit">V</span></td></tr>
    <tr id="llBC"><td class="dlbl">Line-Line BC</td><td class="dval"><span id="v_bc">--</span><span class="dunit">V</span></td></tr>
    <tr id="llCA"><td class="dlbl">Line-Line CA</td><td class="dval"><span id="v_ca">--</span><span class="dunit">V</span></td></tr>
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
    <tr id="ambRow" style="display:none"><td class="dlbl">Ambient Temp</td><td class="dval"><span id="v_amb">--</span><span class="dunit">&#176;C</span></td></tr>
   </table>
  </div>
 </div>
</div>

<!-- Alarms -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10.29 3.86L1.82 18a2 2 0 001.71 3h16.94a2 2 0 001.71-3L13.71 3.86a2 2 0 00-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg><h2>Alarms</h2></div>
  <div class="card-body">
   <div class="ann-summary" id="annSummary"></div>
   <button class="ann-toggle" id="annToggle" onclick="toggleAnnFull()">
    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="6 9 12 15 18 9"/></svg>
    Show all indicators
   </button>
   <div class="ann-full" id="annFull"><div class="ann-grid" id="annPanel"></div></div>
  </div>
 </div>
</div>

<!-- I/O -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="2" y="3" width="20" height="18" rx="2"/><path d="M8 7h8m-8 4h8m-8 4h5"/></svg><h2>Inputs / Outputs</h2></div>
  <div class="card-body"><div class="io-grid" id="ioPanel"></div></div>
 </div>
</div>
</div><!-- /sec-monitor -->

<div class="section" id="sec-config">
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

<!-- Board Relays -->
<div class="row r-full" id="relaySection" style="display:none">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="1" y="4" width="22" height="16" rx="2"/><path d="M7 8v8m5-8v8m5-8v8"/></svg><h2>Board Relays</h2></div>
  <div class="card-body"><div class="io-grid" id="relayPanel"></div></div>
 </div>
</div>

<!-- Exercise Schedule -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="3" y="4" width="18" height="18" rx="2"/><path d="M16 2v4M8 2v4M3 10h18"/><path d="M8 14h.01M12 14h.01M16 14h.01M8 18h.01M12 18h.01"/></svg><h2>Exercise Schedule</h2></div>
  <div class="card-body">
   <div class="ex-form" id="exForm">
    <div class="ex-row">
     <div class="ex-toggle">
      <label class="toggle"><input type="checkbox" id="exEnabled"><span class="slider"></span></label>
      <span id="exEnabledLbl">Disabled</span>
     </div>
    </div>
    <div class="ex-field">
     <label>Day</label>
     <select id="exDay">
      <option value="0">Sunday</option><option value="1">Monday</option><option value="2">Tuesday</option>
      <option value="3">Wednesday</option><option value="4">Thursday</option><option value="5">Friday</option>
      <option value="6">Saturday</option><option value="7">Every Day</option>
     </select>
    </div>
    <div class="ex-field">
     <label>Duration (minutes)</label>
     <input type="number" id="exDuration" min="1" max="120" value="15">
    </div>
    <div class="ex-field">
     <label>Hour (0-23)</label>
     <input type="number" id="exHour" min="0" max="23" value="10">
    </div>
    <div class="ex-field">
     <label>Minute (0-59)</label>
     <input type="number" id="exMinute" min="0" max="59" value="0">
    </div>
    <div class="ex-row">
     <div class="ex-toggle">
      <label class="toggle"><input type="checkbox" id="exXfer"><span class="slider"></span></label>
      <span>Transfer Load During Exercise</span>
     </div>
    </div>
    <div class="ex-btns">
     <button class="ex-btn ex-btn-save" onclick="saveExercise()">Save</button>
     <button class="ex-btn ex-btn-run" id="exRunBtn" onclick="runExercise()">Run Now</button>
     <button class="ex-btn ex-btn-stop" id="exStopBtn" onclick="stopExercise()" style="display:none">Stop</button>
    </div>
    <div class="ex-status" id="exStatus">
     <span class="ex-st-label">Status:</span><span class="ex-st-value" id="exState">Idle</span>
     <span class="ex-st-label" style="margin-left:16px">Last Run:</span><span class="ex-st-value" id="exLastRun">Never</span>
    </div>
   </div>
  </div>
 </div>
</div>

<!-- Relay Thermostat -->
<div class="row r-full" id="thermostatSection" style="display:none">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M14 14.76V3.5a2.5 2.5 0 00-5 0v11.26a4.5 4.5 0 105 0z"/></svg><h2>Relay Thermostat</h2></div>
  <div class="card-body">
   <div id="thermostatPanel"></div>
   <div style="margin-top:12px;text-align:right">
    <button class="ex-btn ex-btn-save" onclick="saveThermostat()">Save</button>
   </div>
  </div>
 </div>
</div>
</div><!-- /sec-config -->

<div class="section" id="sec-history">
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
<!-- Event Log -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/></svg><h2>Event Log</h2></div>
  <div class="card-body">
   <div class="evtlog-bar">
    <span class="evtlog-count" id="evtCount"></span>
    <button class="evtlog-clear" onclick="clearEventLog()">Clear Log</button>
   </div>
   <div id="evtlogWrap" class="evtlog-wrap">
    <div class="evtlog-empty" id="evtEmpty">No events recorded</div>
   </div>
  </div>
 </div>
</div>
</div><!-- /sec-history -->
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
   <text class="gauge-unit" x="70" y="92">${unit}</text>
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
 {k:'on_load',l:'Xfer Closed',c:'green'},{k:'auto_mode',l:'Auto Mode',c:'blue'},{k:'manual_mode',l:'Manual Mode',c:'amber'},
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
 // Auto-hide unused phases (split-phase: A+B=240V, 3-phase: A+B+C)
 const hasC=s.gen_vc!=null&&s.gen_vc>0;
 const hide=(id,show)=>{const e=document.getElementById(id);if(e)e.style.display=show?'':'none';};
 hide('phC',hasC);
 const pg=document.querySelector('.phases');if(pg)pg.style.gridTemplateColumns=hasC?'1fr 1fr 1fr':'1fr 1fr';
 hide('llBC',hasC);hide('llCA',hasC);
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
 // Annunciator — active alarms summary + full grid
 let sdCount=0,wCount=0;
 let activeHtml='';
 ANN.forEach(a=>{
  const on=b[a.k]===true;
  if(on){
   if(a.t==='sd')sdCount++;else wCount++;
   const icon=a.t==='sd'
    ?'<svg class="ann-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><line x1="15" y1="9" x2="9" y2="15"/><line x1="9" y1="9" x2="15" y2="15"/></svg>'
    :'<svg class="ann-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10.29 3.86L1.82 18a2 2 0 001.71 3h16.94a2 2 0 001.71-3L13.71 3.86a2 2 0 00-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg>';
   activeHtml+=`<div class="ann-active ${a.t==='sd'?'sd':'w'}">${icon}<span>${a.l}</span></div>`;
  }
  const lamp=document.getElementById('lmp_'+a.k);
  const row=document.getElementById('ann_'+a.k);
  if(lamp)lamp.className='lamp '+(on?(a.t==='w'?'amber':'red'):'off');
  if(row)row.className='ann'+(on?(a.t==='w'?' active-w':' active-sd'):'');
 });
 const summary=document.getElementById('annSummary');
 if(summary){
  if(sdCount===0&&wCount===0){
   summary.innerHTML='<div class="ann-ok"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"/><polyline points="22 4 12 14.01 9 11.01"/></svg>All systems normal — no active alarms</div>';
  } else {
   summary.innerHTML='<div class="ann-active-list">'+activeHtml+'</div>';
  }
 }
 // Header alarm badge
 const ab=document.getElementById('alarmBadge');
 if(b.estop){ab.textContent='E-STOP';ab.className='badge badge-sd';}
 else if(sdCount>0){ab.textContent=sdCount+' SHUTDOWN';ab.className='badge badge-sd';}
 else if(wCount>0){ab.textContent=wCount+' WARNING';ab.className='badge badge-warn';}
 else{ab.textContent='OK';ab.className='badge badge-ok';}
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
 // Ambient temperature
 if(d.ambient_temp!=null){
  const ar=document.getElementById('ambRow');if(ar)ar.style.display='';
  const ae=document.getElementById('v_amb');if(ae)ae.textContent=f(d.ambient_temp,1);
 }
 // Relays
 if(d.relays){
  if(!RELAYS.length)initRelays(d.relays);
  updateRelays(d.relays);
 }
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

/* ── Relay toggle ── */
let RELAYS=[];
function initRelays(rels){
 if(!rels||!rels.length)return;
 RELAYS=rels;
 const sec=document.getElementById('relaySection');
 if(sec)sec.style.display='';
 let h='';
 rels.forEach((r,i)=>{
  h+=`<div class="relay-item" id="rly_${i}"><span class="relay-lbl">${r.name}</span><label class="toggle"><input type="checkbox" id="rly_cb_${i}" onchange="toggleRelay(${i},this.checked)"><span class="slider"></span></label><span class="relay-state" id="rly_st_${i}">OFF</span></div>`;
 });
 document.getElementById('relayPanel').innerHTML=h;
}
function updateRelays(rels){
 if(!rels||!rels.length)return;
 rels.forEach((r,i)=>{
  const on=!!r.state;
  const row=document.getElementById('rly_'+i);
  const cb=document.getElementById('rly_cb_'+i);
  const st=document.getElementById('rly_st_'+i);
  if(row)row.className='relay-item'+(on?' relay-on':'');
  if(cb&&cb.checked!==on)cb.checked=on;
  if(st)st.textContent=on?'ON':'OFF';
 });
}
function toggleRelay(idx,on){
 fetch('/api/relay',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({relay:idx,state:on})})
 .then(r=>r.json()).then(d=>{
  if(!d.ok)toast('Relay command failed','err');
  setTimeout(poll,300);
 }).catch(()=>toast('Communication error','err'));
}

/* ── Exercise Schedule ── */
let exLoaded=false;
function loadExercise(){
 fetch('/api/exercise').then(r=>r.json()).then(d=>{
  document.getElementById('exEnabled').checked=d.enabled;
  document.getElementById('exEnabledLbl').textContent=d.enabled?'Enabled':'Disabled';
  document.getElementById('exDay').value=d.day;
  document.getElementById('exHour').value=d.hour;
  document.getElementById('exMinute').value=d.minute;
  document.getElementById('exDuration').value=d.duration;
  document.getElementById('exXfer').checked=!!d.load_transfer;
  updateExStatus(d);
  exLoaded=true;
 }).catch(()=>{});
}
function updateExStatus(d){
 const stEl=document.getElementById('exState');
 const failed=d.state==='failed';
 const active=d.state&&d.state!=='idle'&&!failed;
 if(failed){
  let txt='FAILED';
  if(d.fail_reason)txt+=': '+d.fail_reason;
  stEl.textContent=txt;
  stEl.style.color='var(--red)';
  document.getElementById('exRunBtn').style.display='';
  document.getElementById('exStopBtn').style.display='none';
 } else if(active){
  let txt=d.state.charAt(0).toUpperCase()+d.state.slice(1);
  if(d.remaining>0)txt+=' ('+Math.floor(d.remaining/60)+'m '+String(d.remaining%60).padStart(2,'0')+'s remaining)';
  stEl.textContent=txt;
  stEl.style.color='var(--green)';
  document.getElementById('exRunBtn').style.display='none';
  document.getElementById('exStopBtn').style.display='';
 } else {
  stEl.textContent='Idle';
  stEl.style.color='var(--dim)';
  document.getElementById('exRunBtn').style.display='';
  document.getElementById('exStopBtn').style.display='none';
 }
 document.getElementById('exLastRun').textContent=d.last_run||'Never';
}
function pollExercise(){
 if(!exLoaded)return;
 fetch('/api/exercise').then(r=>r.json()).then(d=>updateExStatus(d)).catch(()=>{});
}
function saveExercise(){
 const cfg={
  enabled:document.getElementById('exEnabled').checked,
  day:parseInt(document.getElementById('exDay').value),
  hour:parseInt(document.getElementById('exHour').value),
  minute:parseInt(document.getElementById('exMinute').value),
  duration:parseInt(document.getElementById('exDuration').value),
  load_transfer:document.getElementById('exXfer').checked
 };
 fetch('/api/exercise',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(cfg)})
 .then(r=>r.json()).then(d=>{toast(d.ok?'Exercise config saved':'Save failed',d.ok?'ok':'err');loadExercise();})
 .catch(()=>toast('Communication error','err'));
}
function runExercise(){
 fetch('/api/exercise',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'start'})})
 .then(r=>r.json()).then(d=>{toast(d.ok?'Exercise started':'Failed: '+(d.error||''),d.ok?'ok':'err');})
 .catch(()=>toast('Communication error','err'));
}
function stopExercise(){
 fetch('/api/exercise',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'stop'})})
 .then(r=>r.json()).then(d=>{toast(d.ok?'Exercise stopped':'Failed',d.ok?'ok':'err');})
 .catch(()=>toast('Communication error','err'));
}
document.getElementById('exEnabled').addEventListener('change',function(){
 document.getElementById('exEnabledLbl').textContent=this.checked?'Enabled':'Disabled';
});

/* ── Section switching ── */
function switchSec(id){
 document.querySelectorAll('.section').forEach(s=>s.classList.remove('active'));
 document.querySelectorAll('.nav-tab').forEach(t=>t.classList.remove('active'));
 const sec=document.getElementById('sec-'+id);
 if(sec)sec.classList.add('active');
 const tab=document.querySelector('.nav-tab[data-sec=\"'+id+'\"]');
 if(tab)tab.classList.add('active');
}

/* ── Relay Thermostat ── */
let thData=null;
let thSensors=[];
function loadThermostat(){
 fetch('/api/thermostat').then(r=>r.json()).then(d=>{
  thData=d.relays||[];
  thSensors=d.sensors||[];
  if(thData.length>0)document.getElementById('thermostatSection').style.display='';
  renderThermostat();
 }).catch(()=>{});
}
function renderThermostat(){
 const p=document.getElementById('thermostatPanel');
 if(!p||!thData)return;
 let h='';
 thData.forEach(r=>{
  const active=r.enabled?'th-active':'';
  const rdCls=r.relay_on?'th-on':'th-off';
  const rdTxt=r.reading!=null?r.reading.toFixed(1)+'°':'--';
  const relSt=r.relay_on?'ON':'OFF';
  h+='<div class="th-relay '+active+'" data-idx="'+r.idx+'">';
  h+='<div class="th-hdr"><span class="th-name"><label class="toggle"><input type="checkbox" class="th-en" data-idx="'+r.idx+'"'+(r.enabled?' checked':'')+' onchange="thToggle(this)"><span class="slider"></span></label><span>'+r.name+'</span></span>';
  h+='<span class="th-reading '+rdCls+'">'+rdTxt+' ('+relSt+')</span></div>';
  h+='<div class="th-fields">';
  h+='<div class="th-field"><label>Sensor</label><select class="th-sensor" data-idx="'+r.idx+'">';
  h+='<option value="">-- None --</option>';
  thSensors.forEach(s=>{
   h+='<option value="'+s.key+'"'+(r.sensor===s.key?' selected':'')+'>'+s.label+'</option>';
  });
  h+='</select></div>';
  h+='<div class="th-field"><label>ON Below (°)</label><input type="number" class="th-on" data-idx="'+r.idx+'" step="0.5" value="'+r.on_below.toFixed(1)+'"></div>';
  h+='<div class="th-field"><label>OFF Above (°)</label><input type="number" class="th-off" data-idx="'+r.idx+'" step="0.5" value="'+r.off_above.toFixed(1)+'"></div>';
  h+='</div></div>';
 });
 p.innerHTML=h;
}
function thToggle(el){
 const idx=parseInt(el.dataset.idx);
 const item=thData.find(r=>r.idx===idx);
 if(item)item.enabled=el.checked;
 const card=el.closest('.th-relay');
 if(card){if(el.checked)card.classList.add('th-active');else card.classList.remove('th-active');}
}
function pollThermostat(){
 if(!thData||thData.length===0)return;
 fetch('/api/thermostat').then(r=>r.json()).then(d=>{
  const fresh=d.relays||[];
  fresh.forEach(f=>{
   const old=thData.find(r=>r.idx===f.idx);
   if(old){old.reading=f.reading;old.relay_on=f.relay_on;}
  });
  // Update readings only
  thData.forEach(r=>{
   const card=document.querySelector('.th-relay[data-idx="'+r.idx+'"]');
   if(!card)return;
   const rd=card.querySelector('.th-reading');
   if(rd){
    rd.textContent=(r.reading!=null?r.reading.toFixed(1)+'°':'--')+' ('+(r.relay_on?'ON':'OFF')+')';
    rd.className='th-reading '+(r.relay_on?'th-on':'th-off');
   }
  });
 }).catch(()=>{});
}
function saveThermostat(){
 const cfg=thData.map(r=>{
  const el_en=document.querySelector('.th-en[data-idx="'+r.idx+'"]');
  const el_s=document.querySelector('.th-sensor[data-idx="'+r.idx+'"]');
  const el_on=document.querySelector('.th-on[data-idx="'+r.idx+'"]');
  const el_off=document.querySelector('.th-off[data-idx="'+r.idx+'"]');
  return {
   idx:r.idx,
   enabled:el_en?el_en.checked:false,
   sensor:el_s?el_s.value:'',
   on_below:el_on?parseFloat(el_on.value):5.0,
   off_above:el_off?parseFloat(el_off.value):10.0
  };
 });
 fetch('/api/thermostat',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(cfg)})
 .then(r=>r.json()).then(d=>{toast(d.ok?'Thermostat config saved':'Save failed',d.ok?'ok':'err');loadThermostat();})
 .catch(()=>toast('Communication error','err'));
}

/* ── Event Log ── */
function loadEventLog(){
 fetch('/api/eventlog').then(r=>r.json()).then(d=>{
  const wrap=document.getElementById('evtlogWrap');
  const empty=document.getElementById('evtEmpty');
  const cnt=document.getElementById('evtCount');
  if(!d.events||d.events.length===0){
   wrap.innerHTML='<div class="evtlog-empty">No events recorded</div>';
   cnt.textContent='';
   return;
  }
  cnt.textContent=d.events.length+' event'+(d.events.length!==1?'s':'');
  let html='<table class="evtlog-tbl"><thead><tr><th>Time</th><th>Event</th></tr></thead><tbody>';
  d.events.forEach(e=>{
   html+='<tr><td class="evt-ts">'+e.time+'</td><td class="evt-msg">'+e.msg.replace(/</g,'&lt;')+'</td></tr>';
  });
  html+='</tbody></table>';
  wrap.innerHTML=html;
 }).catch(()=>{});
}
function clearEventLog(){
 if(!confirm('Clear all event log entries?'))return;
 fetch('/api/eventlog',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'clear'})})
 .then(r=>r.json()).then(d=>{toast(d.ok?'Event log cleared':'Clear failed',d.ok?'ok':'err');loadEventLog();})
 .catch(()=>toast('Communication error','err'));
}

/* ── Init ── */
initGauges();initPanels();poll();setInterval(poll,2500);
loadExercise();setInterval(pollExercise,3000);
loadThermostat();setInterval(pollThermostat,5000);
loadEventLog();setInterval(loadEventLog,10000);

function toggleAnnFull(){
 const f=document.getElementById('annFull');
 const t=document.getElementById('annToggle');
 f.classList.toggle('show');
 t.classList.toggle('open');
 t.querySelector('svg').nextSibling.textContent=f.classList.contains('show')?' Hide indicators':' Show all indicators';
}
</script>
</body>
</html>)rawliteral";

// ============================================================
//  Component lifecycle
// ============================================================

void SmartgenHSC941Web::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SmartGen HSC941 Web UI...");
  this->init_spiffs_();
  this->load_event_log_();
  this->load_exercise_config_();
  this->load_thermostat_config_();
  this->start_server_();
  this->log_event("System booted");
}

void SmartgenHSC941Web::loop() {
  uint32_t now = millis();
  // Check exercise schedule every 1 second
  if (now - this->last_exercise_check_ >= 1000) {
    this->last_exercise_check_ = now;
    if (this->exercise_state_ != ExerciseState::IDLE) {
      this->exercise_step_();
    } else {
      this->check_exercise_schedule_();
    }
  }
  // Check thermostat every 5 seconds
  if (now - this->last_thermostat_check_ >= 5000) {
    this->last_thermostat_check_ = now;
    this->thermostat_step_();
  }
  // Check alarm transitions every 2 seconds
  if (now - this->last_alarm_check_ >= 2000) {
    this->last_alarm_check_ = now;
    this->check_alarm_transitions_();
  }
}

void SmartgenHSC941Web::dump_config() {
  ESP_LOGCONFIG(TAG, "SmartGen HSC941 Web UI:");
  ESP_LOGCONFIG(TAG, "  Port: %u", this->port_);
  if (this->server_) {
    ESP_LOGCONFIG(TAG, "  Server: running");
  } else {
    ESP_LOGCONFIG(TAG, "  Server: FAILED to start");
  }
  ESP_LOGCONFIG(TAG, "  Exercise: %s", this->exercise_cfg_.enabled ? "enabled" : "disabled");
  if (this->exercise_cfg_.enabled) {
    const char *days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat","Daily"};
    ESP_LOGCONFIG(TAG, "  Exercise schedule: %s %02u:%02u for %u min",
                  days[this->exercise_cfg_.day > 7 ? 7 : this->exercise_cfg_.day],
                  this->exercise_cfg_.hour, this->exercise_cfg_.minute,
                  this->exercise_cfg_.duration_min);
  }
}

// ============================================================
//  Exercise NVS persistence
// ============================================================
static const char *NVS_NAMESPACE = "genset_ex";
static const char *NVS_KEY = "config";

void SmartgenHSC941Web::load_exercise_config_() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
  if (err != ESP_OK) {
    ESP_LOGD(TAG, "No exercise config in NVS (first boot)");
    return;
  }
  size_t len = 0;
  err = nvs_get_str(handle, NVS_KEY, nullptr, &len);
  if (err == ESP_OK && len > 0) {
    char *buf = new char[len];
    nvs_get_str(handle, NVS_KEY, buf, &len);
    // Simple parse: {"enabled":true,"day":1,"hour":10,"minute":0,"duration":15,"load_transfer":false}
    auto find_int = [&](const char *key) -> int {
      const char *p = strstr(buf, key);
      if (!p) return -1;
      p = strchr(p, ':');
      if (!p) return -1;
      return atoi(p + 1);
    };
    this->exercise_cfg_.enabled = (strstr(buf, "\"enabled\":true") != nullptr);
    int v;
    if ((v = find_int("\"day\"")) >= 0) this->exercise_cfg_.day = v;
    if ((v = find_int("\"hour\"")) >= 0) this->exercise_cfg_.hour = v;
    if ((v = find_int("\"minute\"")) >= 0) this->exercise_cfg_.minute = v;
    if ((v = find_int("\"duration\"")) >= 0) this->exercise_cfg_.duration_min = v;
    this->exercise_cfg_.load_transfer = (strstr(buf, "\"load_transfer\":true") != nullptr);
    delete[] buf;
    ESP_LOGI(TAG, "Loaded exercise config: %s day=%u %02u:%02u dur=%umin xfer=%s",
             this->exercise_cfg_.enabled ? "ON" : "OFF",
             this->exercise_cfg_.day, this->exercise_cfg_.hour, this->exercise_cfg_.minute,
             this->exercise_cfg_.duration_min,
             this->exercise_cfg_.load_transfer ? "yes" : "no");
  }
  nvs_close(handle);
}

void SmartgenHSC941Web::save_exercise_config_() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to open NVS for writing: %s", esp_err_to_name(err));
    return;
  }
  char json[192];
  snprintf(json, sizeof(json),
           "{\"enabled\":%s,\"day\":%u,\"hour\":%u,\"minute\":%u,\"duration\":%u,\"load_transfer\":%s}",
           this->exercise_cfg_.enabled ? "true" : "false",
           this->exercise_cfg_.day, this->exercise_cfg_.hour, this->exercise_cfg_.minute,
           this->exercise_cfg_.duration_min,
           this->exercise_cfg_.load_transfer ? "true" : "false");
  nvs_set_str(handle, NVS_KEY, json);
  nvs_commit(handle);
  nvs_close(handle);
  ESP_LOGI(TAG, "Saved exercise config: %s", json);
}

// ============================================================
//  Thermostat NVS persistence
// ============================================================
static const char *NVS_THERMO_NS = "genset_th";
static const char *NVS_THERMO_KEY = "config";

float SmartgenHSC941Web::get_sensor_value_by_key_(const std::string &key) const {
  if (key == "water_temp" && this->controller_) {
    auto *s = this->controller_->get_water_temp_sensor();
    if (s && s->has_state()) return s->state;
  } else if (key == "aux1" && this->controller_) {
    auto *s = this->controller_->get_aux_sensor_1();
    if (s && s->has_state()) return s->state;
  } else if (key == "ambient_temp" && this->ambient_temp_) {
    if (this->ambient_temp_->has_state()) return this->ambient_temp_->state;
  }
  return NAN;
}

void SmartgenHSC941Web::load_thermostat_config_() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(NVS_THERMO_NS, NVS_READONLY, &handle);
  if (err != ESP_OK) {
    ESP_LOGD(TAG, "No thermostat config in NVS (first boot)");
    return;
  }
  size_t len = 0;
  err = nvs_get_str(handle, NVS_THERMO_KEY, nullptr, &len);
  if (err == ESP_OK && len > 0) {
    char *buf = new char[len];
    nvs_get_str(handle, NVS_THERMO_KEY, buf, &len);
    // Format: array of per-relay configs:
    // [{"e":1,"s":"water_temp","on":5.0,"off":10.0}, ...]
    const char *p = buf;
    uint8_t idx = 0;
    while (idx < MAX_RELAYS) {
      const char *start = strchr(p, '{');
      if (!start) break;
      const char *end = strchr(start, '}');
      if (!end) break;
      this->relay_thermostats_[idx].enabled =
          (strstr(start, "\"e\":1") != nullptr || strstr(start, "\"e\":true") != nullptr);
      const char *sk = strstr(start, "\"s\":\"");
      if (sk && sk < end) {
        sk += 5;
        const char *sq = strchr(sk, '"');
        if (sq && sq < end) {
          this->relay_thermostats_[idx].sensor_key = std::string(sk, sq - sk);
        }
      }
      const char *onp = strstr(start, "\"on\":");
      if (onp && onp < end) {
        this->relay_thermostats_[idx].on_below = strtof(onp + 5, nullptr);
      }
      const char *offp = strstr(start, "\"off\":");
      if (offp && offp < end) {
        this->relay_thermostats_[idx].off_above = strtof(offp + 6, nullptr);
      }
      idx++;
      p = end + 1;
    }
    delete[] buf;
    ESP_LOGI(TAG, "Loaded thermostat config for %u relays", idx);
  }
  nvs_close(handle);
}

void SmartgenHSC941Web::save_thermostat_config_() {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(NVS_THERMO_NS, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to open NVS for thermostat write: %s", esp_err_to_name(err));
    return;
  }
  std::string json;
  json.reserve(512);
  json += '[';
  for (uint8_t i = 0; i < MAX_RELAYS; i++) {
    if (i > 0) json += ',';
    const auto &t = this->relay_thermostats_[i];
    json += "{\"e\":";
    json += t.enabled ? "1" : "0";
    json += ",\"s\":\"";
    json += t.sensor_key;
    json += "\",\"on\":";
    char fbuf[16];
    snprintf(fbuf, sizeof(fbuf), "%.1f", t.on_below);
    json += fbuf;
    json += ",\"off\":";
    snprintf(fbuf, sizeof(fbuf), "%.1f", t.off_above);
    json += fbuf;
    json += '}';
  }
  json += ']';
  nvs_set_str(handle, NVS_THERMO_KEY, json.c_str());
  nvs_commit(handle);
  nvs_close(handle);
  ESP_LOGI(TAG, "Saved thermostat config");
}

void SmartgenHSC941Web::thermostat_step_() {
  for (uint8_t i = 0; i < MAX_RELAYS; i++) {
    const auto &t = this->relay_thermostats_[i];
    if (!t.enabled || t.sensor_key.empty()) continue;
    if (this->relays_[i].sw == nullptr) continue;

    float val = this->get_sensor_value_by_key_(t.sensor_key);
    if (std::isnan(val)) continue;

    bool currently_on = this->relays_[i].sw->state;
    if (!currently_on && val < t.on_below) {
      this->relays_[i].sw->turn_on();
      ESP_LOGI(TAG, "Thermostat: Relay %u ON (%.1f < %.1f)", i + 1, val, t.on_below);
      char msg[64];
      snprintf(msg, sizeof(msg), "Thermostat: %s ON (%.1f\u00b0 < %.1f\u00b0)",
               this->relays_[i].name.c_str(), val, t.on_below);
      this->log_event(msg);
    } else if (currently_on && val > t.off_above) {
      this->relays_[i].sw->turn_off();
      ESP_LOGI(TAG, "Thermostat: Relay %u OFF (%.1f > %.1f)", i + 1, val, t.off_above);
      char msg[64];
      snprintf(msg, sizeof(msg), "Thermostat: %s OFF (%.1f\u00b0 > %.1f\u00b0)",
               this->relays_[i].name.c_str(), val, t.off_above);
      this->log_event(msg);
    }
  }
}

// ============================================================
//  Event Log — SPIFFS-backed persistent log
// ============================================================
static const char *SPIFFS_PARTITION = "eventlog";
static const char *SPIFFS_BASE_PATH = "/eventlog";
static const char *LOG_FILE_PATH = "/eventlog/events.log";
// Max file size ~192KB (leave headroom in 256KB partition)
static const size_t MAX_LOG_FILE_SIZE = 192 * 1024;

void SmartgenHSC941Web::init_spiffs_() {
  esp_vfs_spiffs_conf_t conf = {
    .base_path = SPIFFS_BASE_PATH,
    .partition_label = SPIFFS_PARTITION,
    .max_files = 2,
    .format_if_mount_failed = true,
  };
  esp_err_t err = esp_vfs_spiffs_register(&conf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to mount SPIFFS '%s': %s", SPIFFS_PARTITION, esp_err_to_name(err));
    return;
  }
  this->spiffs_mounted_ = true;

  size_t total = 0, used = 0;
  esp_spiffs_info(SPIFFS_PARTITION, &total, &used);
  ESP_LOGI(TAG, "SPIFFS '%s' mounted: %u/%u bytes used", SPIFFS_PARTITION, used, total);
}

void SmartgenHSC941Web::load_event_log_() {
  if (!this->spiffs_mounted_) return;

  FILE *f = fopen(LOG_FILE_PATH, "r");
  if (!f) {
    ESP_LOGD(TAG, "No event log file (first boot)");
    return;
  }

  char line[256];
  uint16_t count = 0;
  while (fgets(line, sizeof(line), f) != nullptr) {
    // Format: <timestamp>\t<message>\n
    char *tab = strchr(line, '\t');
    if (!tab) continue;
    *tab = '\0';
    char *msg = tab + 1;
    // Strip trailing newline
    char *nl = strchr(msg, '\n');
    if (nl) *nl = '\0';

    EventEntry entry;
    entry.timestamp = (time_t)strtoll(line, nullptr, 10);
    entry.message = msg;
    this->event_log_.push_back(std::move(entry));
    count++;
  }
  fclose(f);

  // If log exceeds capacity, trim oldest
  if (this->event_log_.size() > MAX_LOG_ENTRIES) {
    size_t excess = this->event_log_.size() - MAX_LOG_ENTRIES;
    this->event_log_.erase(this->event_log_.begin(), this->event_log_.begin() + excess);
  }

  ESP_LOGI(TAG, "Loaded %u events from log file", count);
}

void SmartgenHSC941Web::append_event_to_file_(const std::string &message, time_t ts) {
  if (!this->spiffs_mounted_) return;

  // Check file size — if too large, truncate to last half
  struct stat st;
  if (stat(LOG_FILE_PATH, &st) == 0 && (size_t)st.st_size > MAX_LOG_FILE_SIZE) {
    ESP_LOGW(TAG, "Event log file %.1fKB > %.1fKB limit, truncating",
             st.st_size / 1024.0f, MAX_LOG_FILE_SIZE / 1024.0f);
    // Read last half of file
    FILE *f = fopen(LOG_FILE_PATH, "r");
    if (f) {
      fseek(f, st.st_size / 2, SEEK_SET);
      // Skip to next newline to avoid partial line
      char ch;
      while (fread(&ch, 1, 1, f) == 1 && ch != '\n') {}
      // Read remainder
      std::string remainder;
      char buf[512];
      size_t n;
      while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        remainder.append(buf, n);
      }
      fclose(f);
      // Rewrite file with remainder
      f = fopen(LOG_FILE_PATH, "w");
      if (f) {
        fwrite(remainder.data(), 1, remainder.size(), f);
        fclose(f);
      }
    }
  }

  FILE *f = fopen(LOG_FILE_PATH, "a");
  if (!f) {
    ESP_LOGE(TAG, "Failed to open event log for append");
    return;
  }
  fprintf(f, "%lld\t%s\n", (long long)ts, message.c_str());
  fclose(f);
}

void SmartgenHSC941Web::clear_event_log_file_() {
  this->event_log_.clear();
  if (this->spiffs_mounted_) {
    unlink(LOG_FILE_PATH);
    ESP_LOGI(TAG, "Event log cleared");
  }
}

void SmartgenHSC941Web::log_event(const std::string &message) {
  time_t now_t = time(nullptr);
  EventEntry entry;
  entry.timestamp = now_t;
  entry.message = message;
  this->event_log_.push_back(std::move(entry));

  // Trim if over capacity
  if (this->event_log_.size() > MAX_LOG_ENTRIES) {
    this->event_log_.erase(this->event_log_.begin());
  }

  this->append_event_to_file_(message, now_t);
  ESP_LOGI(TAG, "Event: %s", message.c_str());
}

void SmartgenHSC941Web::check_alarm_transitions_() {
  if (!this->controller_) return;

  bool connected = this->controller_->is_connected();
  bool shutdown = this->controller_->is_any_shutdown();
  bool engine_running = this->controller_->is_engine_running();
  bool auto_mode = this->controller_->is_in_auto_mode();
  bool manual_mode = this->controller_->is_in_manual_mode();
  bool stop_mode = this->controller_->is_in_stop_mode();
  bool estop = this->controller_->is_emergency_stop();
  bool gen_on_load = this->controller_->is_gen_on_load();

  // Connection state change
  if (connected != this->prev_connected_) {
    this->log_event(connected ? "Controller communication restored" : "Controller communication lost");
    this->prev_connected_ = connected;
  }

  if (!connected) return;  // Don't log sensor transitions when disconnected

  // Shutdown state change
  if (shutdown != this->prev_shutdown_) {
    this->log_event(shutdown ? "SHUTDOWN condition active" : "Shutdown condition cleared");
    this->prev_shutdown_ = shutdown;
  }

  // Emergency stop
  if (estop != this->prev_estop_) {
    this->log_event(estop ? "EMERGENCY STOP activated" : "Emergency stop cleared");
    this->prev_estop_ = estop;
  }

  // Engine running state change
  if (engine_running != this->prev_engine_running_) {
    if (engine_running) {
      char msg[48];
      snprintf(msg, sizeof(msg), "Engine started (%.0f RPM)", this->controller_->get_engine_rpm());
      this->log_event(msg);
    } else {
      this->log_event("Engine stopped");
    }
    this->prev_engine_running_ = engine_running;
  }

  // Controller mode changes (physical panel buttons)
  if (auto_mode != this->prev_auto_mode_) {
    if (auto_mode) this->log_event("Panel: switched to AUTO mode");
    this->prev_auto_mode_ = auto_mode;
  }
  if (manual_mode != this->prev_manual_mode_) {
    if (manual_mode) this->log_event("Panel: switched to MANUAL mode");
    this->prev_manual_mode_ = manual_mode;
  }
  if (stop_mode != this->prev_stop_mode_) {
    if (stop_mode) this->log_event("Panel: switched to STOP mode");
    this->prev_stop_mode_ = stop_mode;
  }

  // Generator load state change (transfer switch)
  if (gen_on_load != this->prev_gen_on_load_) {
    this->log_event(gen_on_load ? "Generator ON LOAD (transfer switch closed)" : "Generator OFF LOAD (transfer switch opened)");
    this->prev_gen_on_load_ = gen_on_load;
  }
}

// ============================================================
//  Exercise scheduler logic
// ============================================================

// Timeout for engine to reach running RPM after start command (90 seconds
// covers typical 3-attempt crank cycle on HSC941)
static const uint32_t CRANK_TIMEOUT_MS = 90000;
// Time to wait after opening transfer switch before sending stop (ms)
static const uint32_t COOLDOWN_DELAY_MS = 15000;
// Time to wait after stop command for engine to spin down (ms)
static const uint32_t STOP_WAIT_MS = 30000;
// Time to display FAILED state before auto-clearing to IDLE (ms)
static const uint32_t FAIL_DISPLAY_MS = 60000;

uint32_t SmartgenHSC941Web::get_exercise_remaining_sec() const {
  if (this->exercise_state_ != ExerciseState::RUNNING) return 0;
  uint32_t elapsed_ms = millis() - this->exercise_run_start_;
  uint32_t total_ms = this->exercise_cfg_.duration_min * 60U * 1000U;
  if (elapsed_ms >= total_ms) return 0;
  return (total_ms - elapsed_ms) / 1000;
}

void SmartgenHSC941Web::check_exercise_schedule_() {
  if (!this->exercise_cfg_.enabled) return;
  if (!this->controller_ || !this->controller_->is_connected()) return;

  time_t now_t = time(nullptr);
  if (now_t < 1704067200) return;  // Time not synced yet (before 2024-01-01)
  struct tm *now_tm = localtime(&now_t);
  if (!now_tm) return;

  uint8_t wday = now_tm->tm_wday;  // 0=Sun
  uint8_t hour = now_tm->tm_hour;
  uint8_t minute = now_tm->tm_min;

  // Reset trigger guard on new day
  if (wday != this->exercise_last_trigger_day_) {
    this->exercise_triggered_today_ = false;
    this->exercise_last_trigger_day_ = wday;
  }

  if (this->exercise_triggered_today_) return;

  // Don't start if engine is already running (e.g. power outage auto-start)
  if (this->controller_->is_engine_running()) return;

  // Check if schedule matches
  bool day_match = (this->exercise_cfg_.day == 7) || (this->exercise_cfg_.day == wday);
  if (day_match && hour == this->exercise_cfg_.hour && minute == this->exercise_cfg_.minute) {
    this->exercise_triggered_today_ = true;
    ESP_LOGI(TAG, "Exercise schedule triggered!");
    this->start_exercise_();
  }
}

void SmartgenHSC941Web::start_exercise_() {
  if (this->exercise_state_ != ExerciseState::IDLE) return;
  if (!this->controller_) return;

  ESP_LOGI(TAG, "Starting exercise: %u minutes, load_transfer=%s",
           this->exercise_cfg_.duration_min,
           this->exercise_cfg_.load_transfer ? "yes" : "no");

  char ex_msg[64];
  snprintf(ex_msg, sizeof(ex_msg), "Exercise started (%u min, xfer=%s)",
           this->exercise_cfg_.duration_min,
           this->exercise_cfg_.load_transfer ? "on" : "off");
  this->log_event(ex_msg);

  this->exercise_fail_reason_.clear();
  this->exercise_start_cmd_sent_ = false;

  // Switch to manual mode first
  this->controller_->write_coil(4, true);  // Manual mode
  this->exercise_state_ = ExerciseState::STARTING;
  this->exercise_phase_start_ = millis();

  // Record last run timestamp
  time_t now_t = time(nullptr);
  struct tm *now_tm = localtime(&now_t);
  if (now_tm) {
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M", now_tm);
    this->exercise_last_run_ = ts;
  }
}

void SmartgenHSC941Web::exercise_step_() {
  if (!this->controller_) {
    this->exercise_state_ = ExerciseState::IDLE;
    return;
  }

  uint32_t phase_elapsed = millis() - this->exercise_phase_start_;

  switch (this->exercise_state_) {

    case ExerciseState::STARTING: {
      // After 3s delay (for manual mode to register), send start command once
      if (!this->exercise_start_cmd_sent_ && phase_elapsed > 3000) {
        this->controller_->write_coil(0, true);  // Start engine
        this->exercise_start_cmd_sent_ = true;
        ESP_LOGI(TAG, "Exercise: start command sent, waiting for engine RPM...");
      }

      // Check for crank failure shutdown
      if (this->exercise_start_cmd_sent_ && this->controller_->is_crank_failure()) {
        ESP_LOGW(TAG, "Exercise: CRANK FAILURE detected, aborting");
        this->log_event("Exercise FAILED: crank failure");
        this->exercise_fail_reason_ = "Crank failure";
        this->exercise_state_ = ExerciseState::FAILED;
        this->exercise_phase_start_ = millis();
        // Try to return to safe state
        this->controller_->write_coil(1, true);  // Stop
        this->controller_->write_coil(3, true);  // Auto mode
        break;
      }

      // Check for any shutdown condition
      if (this->exercise_start_cmd_sent_ && this->controller_->is_any_shutdown()) {
        ESP_LOGW(TAG, "Exercise: SHUTDOWN detected during start, aborting");
        this->log_event("Exercise FAILED: shutdown during start");
        this->exercise_fail_reason_ = "Shutdown during start";
        this->exercise_state_ = ExerciseState::FAILED;
        this->exercise_phase_start_ = millis();
        this->controller_->write_coil(3, true);  // Auto mode
        break;
      }

      // Check if engine is actually running (RPM > threshold)
      if (this->exercise_start_cmd_sent_ && this->controller_->is_engine_running()) {
        ESP_LOGI(TAG, "Exercise: engine running at %.0f RPM, starting %u min timer",
                 this->controller_->get_engine_rpm(), this->exercise_cfg_.duration_min);
        this->log_event("Exercise: engine running");
        this->exercise_state_ = ExerciseState::RUNNING;
        this->exercise_run_start_ = millis();
        this->exercise_phase_start_ = millis();

        // Optionally close transfer switch
        if (this->exercise_cfg_.load_transfer) {
          this->controller_->write_coil(6, true);  // Gen switch on
          ESP_LOGI(TAG, "Exercise: transfer switch closed");
        }
        break;
      }

      // Timeout — engine never reached running RPM
      if (phase_elapsed > CRANK_TIMEOUT_MS) {
        ESP_LOGW(TAG, "Exercise: engine failed to start within %u seconds, aborting",
                 CRANK_TIMEOUT_MS / 1000);
        this->log_event("Exercise FAILED: start timeout (no RPM)");
        this->exercise_fail_reason_ = "Start timeout (no RPM)";
        this->exercise_state_ = ExerciseState::FAILED;
        this->exercise_phase_start_ = millis();
        this->controller_->write_coil(1, true);  // Stop
        this->controller_->write_coil(3, true);  // Auto mode
      }
      break;
    }

    case ExerciseState::RUNNING: {
      // Monitor for unexpected shutdown while running
      if (this->controller_->is_any_shutdown()) {
        ESP_LOGW(TAG, "Exercise: SHUTDOWN detected during run, aborting");
        this->log_event("Exercise FAILED: shutdown during run");
        this->exercise_fail_reason_ = "Shutdown during run";
        this->exercise_state_ = ExerciseState::FAILED;
        this->exercise_phase_start_ = millis();
        if (this->exercise_cfg_.load_transfer) {
          this->controller_->write_coil(5, true);  // Gen switch off
        }
        this->controller_->write_coil(3, true);  // Auto mode
        break;
      }

      // Monitor for engine stall (RPM dropped to zero unexpectedly)
      if (!this->controller_->is_engine_running()) {
        ESP_LOGW(TAG, "Exercise: engine stalled (RPM=%.0f), aborting",
                 this->controller_->get_engine_rpm());
        this->log_event("Exercise FAILED: engine stalled");
        this->exercise_fail_reason_ = "Engine stalled";
        this->exercise_state_ = ExerciseState::FAILED;
        this->exercise_phase_start_ = millis();
        if (this->exercise_cfg_.load_transfer) {
          this->controller_->write_coil(5, true);  // Gen switch off
        }
        this->controller_->write_coil(3, true);  // Auto mode
        break;
      }

      // Check if duration has elapsed
      uint32_t run_elapsed = millis() - this->exercise_run_start_;
      uint32_t duration_ms = this->exercise_cfg_.duration_min * 60U * 1000U;
      if (run_elapsed >= duration_ms) {
        ESP_LOGI(TAG, "Exercise: duration complete, beginning cooldown");
        // Open transfer switch first if it was closed
        if (this->exercise_cfg_.load_transfer) {
          this->controller_->write_coil(5, true);  // Gen switch off
          ESP_LOGI(TAG, "Exercise: transfer switch opened");
        }
        this->exercise_state_ = ExerciseState::COOLDOWN;
        this->exercise_phase_start_ = millis();
      }
      break;
    }

    case ExerciseState::COOLDOWN:
      // Wait for transfer switch to open before stopping engine
      if (phase_elapsed > COOLDOWN_DELAY_MS) {
        this->controller_->write_coil(1, true);  // Stop engine
        this->exercise_state_ = ExerciseState::STOPPING;
        this->exercise_phase_start_ = millis();
        ESP_LOGI(TAG, "Exercise: stop command sent, waiting for engine to stop");
      }
      break;

    case ExerciseState::STOPPING:
      // Wait for engine to actually stop (RPM = 0) or timeout
      if (!this->controller_->is_engine_running() || phase_elapsed > STOP_WAIT_MS) {
        if (this->controller_->is_engine_running()) {
          ESP_LOGW(TAG, "Exercise: engine still running after stop timeout, forcing auto mode");
        }
        this->controller_->write_coil(3, true);  // Auto mode
        this->exercise_state_ = ExerciseState::IDLE;
        this->log_event("Exercise complete");
        ESP_LOGI(TAG, "Exercise: complete, returned to auto mode");
      }
      break;

    case ExerciseState::FAILED:
      // Display failure for a while then auto-clear
      if (phase_elapsed > FAIL_DISPLAY_MS) {
        ESP_LOGI(TAG, "Exercise: clearing failure state");
        this->exercise_state_ = ExerciseState::IDLE;
      }
      break;

    case ExerciseState::IDLE:
      break;
  }
}

void SmartgenHSC941Web::stop_exercise_() {
  if (this->exercise_state_ == ExerciseState::IDLE ||
      this->exercise_state_ == ExerciseState::FAILED) return;
  ESP_LOGW(TAG, "Exercise manually stopped");
  this->log_event("Exercise manually stopped");

  if (this->controller_) {
    // Open transfer switch if it was closed during RUNNING
    if (this->exercise_cfg_.load_transfer &&
        (this->exercise_state_ == ExerciseState::RUNNING)) {
      this->controller_->write_coil(5, true);  // Gen switch off
    }
    this->controller_->write_coil(1, true);  // Stop engine
    this->controller_->write_coil(3, true);  // Auto mode
  }
  this->exercise_state_ = ExerciseState::IDLE;
}

// ============================================================
//  HTTP Server start / stop
// ============================================================

void SmartgenHSC941Web::start_server_() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = this->port_;
  config.ctrl_port = this->port_ + 32768;  // control port offset
  config.stack_size = 8192;
  config.max_uri_handlers = 11;
  config.max_open_sockets = 4;
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

  httpd_uri_t relay_uri = {
      .uri = "/api/relay",
      .method = HTTP_POST,
      .handler = SmartgenHSC941Web::handle_api_relay_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &relay_uri);

  httpd_uri_t exercise_get_uri = {
      .uri = "/api/exercise",
      .method = HTTP_GET,
      .handler = SmartgenHSC941Web::handle_api_exercise_get_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &exercise_get_uri);

  httpd_uri_t exercise_post_uri = {
      .uri = "/api/exercise",
      .method = HTTP_POST,
      .handler = SmartgenHSC941Web::handle_api_exercise_post_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &exercise_post_uri);

  httpd_uri_t thermostat_get_uri = {
      .uri = "/api/thermostat",
      .method = HTTP_GET,
      .handler = SmartgenHSC941Web::handle_api_thermostat_get_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &thermostat_get_uri);

  httpd_uri_t thermostat_post_uri = {
      .uri = "/api/thermostat",
      .method = HTTP_POST,
      .handler = SmartgenHSC941Web::handle_api_thermostat_post_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &thermostat_post_uri);

  httpd_uri_t eventlog_get_uri = {
      .uri = "/api/eventlog",
      .method = HTTP_GET,
      .handler = SmartgenHSC941Web::handle_api_eventlog_get_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &eventlog_get_uri);

  httpd_uri_t eventlog_post_uri = {
      .uri = "/api/eventlog",
      .method = HTTP_POST,
      .handler = SmartgenHSC941Web::handle_api_eventlog_post_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &eventlog_post_uri);
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

  // Append ambient temperature if configured (strip closing '}' and re-close)
  if (self->ambient_temp_ != nullptr) {
    if (!json.empty() && json.back() == '}') json.pop_back();
    json += ",\"ambient_temp\":";
    if (self->ambient_temp_->has_state()) {
      char buf[16];
      snprintf(buf, sizeof(buf), "%.1f", self->ambient_temp_->state);
      json += buf;
    } else {
      json += "null";
    }
    json += '}';
  }

  // Append relay states if any configured
  {
    bool has_any = false;
    for (uint8_t i = 0; i < MAX_RELAYS; i++) {
      if (self->relays_[i].sw != nullptr) { has_any = true; break; }
    }
    if (has_any) {
      if (!json.empty() && json.back() == '}') json.pop_back();
      json += ",\"relays\":[";
      bool first = true;
      for (uint8_t i = 0; i < MAX_RELAYS; i++) {
        if (self->relays_[i].sw == nullptr) continue;
        if (!first) json += ',';
        first = false;
        json += "{\"idx\":";
        json += std::to_string(i);
        json += ",\"name\":\"";
        // Escape relay name for JSON safety
        for (char c : self->relays_[i].name) {
          if (c == '"') json += "\\\"";
          else if (c == '\\') json += "\\\\";
          else json += c;
        }
        json += "\",\"state\":";
        json += self->relays_[i].sw->state ? "true" : "false";
        json += '}';
      }
      json += "]}";
    }
  }

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

  // Log the command event
  if (ok) {
    const char *cmd_name = "Unknown";
    switch (coil) {
      case 0: cmd_name = "Start"; break;
      case 1: cmd_name = "Stop"; break;
      case 3: cmd_name = "Auto mode"; break;
      case 4: cmd_name = "Manual mode"; break;
      case 5: cmd_name = "Transfer open"; break;
      case 6: cmd_name = "Transfer close"; break;
      case 7: cmd_name = "Fault reset"; break;
    }
    char evt[48];
    snprintf(evt, sizeof(evt), "Command: %s", cmd_name);
    self->log_event(evt);
  }

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  const char *resp = ok ? R"({"ok":true})" : R"({"ok":false,"error":"write failed"})";
  return httpd_resp_send(req, resp, strlen(resp));
}

esp_err_t SmartgenHSC941Web::handle_api_relay_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
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

  // Parse {"relay":N,"state":true/false}
  const char *relay_key = strstr(body, "\"relay\"");
  if (!relay_key) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing 'relay' field");
    return ESP_FAIL;
  }
  const char *rcolon = strchr(relay_key, ':');
  if (!rcolon) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
    return ESP_FAIL;
  }
  int idx = atoi(rcolon + 1);
  if (idx < 0 || idx >= MAX_RELAYS || self->relays_[idx].sw == nullptr) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid relay index");
    return ESP_FAIL;
  }

  const char *state_key = strstr(body, "\"state\"");
  bool turn_on = false;
  if (state_key) {
    const char *scolon = strchr(state_key, ':');
    if (scolon) {
      // Skip whitespace
      const char *p = scolon + 1;
      while (*p == ' ') p++;
      turn_on = (*p == 't' || *p == 'T' || *p == '1');
    }
  }

  if (turn_on) {
    self->relays_[idx].sw->turn_on();
  } else {
    self->relays_[idx].sw->turn_off();
  }

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  const char *resp = R"({"ok":true})";
  return httpd_resp_send(req, resp, strlen(resp));
}

// ============================================================
//  Exercise API handlers
// ============================================================

esp_err_t SmartgenHSC941Web::handle_api_exercise_get_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

  const auto &cfg = self->get_exercise_config();
  auto state = self->get_exercise_state();
  uint32_t remain = self->get_exercise_remaining_sec();
  const char *state_str = "idle";
  switch (state) {
    case ExerciseState::STARTING: state_str = "starting"; break;
    case ExerciseState::RUNNING:  state_str = "running"; break;
    case ExerciseState::COOLDOWN: state_str = "cooldown"; break;
    case ExerciseState::STOPPING: state_str = "stopping"; break;
    case ExerciseState::FAILED:   state_str = "failed"; break;
    default: break;
  }

  char json[512];
  snprintf(json, sizeof(json),
    "{\"enabled\":%s,\"day\":%u,\"hour\":%u,\"minute\":%u,\"duration\":%u,"
    "\"load_transfer\":%s,\"state\":\"%s\",\"remaining\":%u,\"last_run\":\"%s\","
    "\"fail_reason\":\"%s\"}",
    cfg.enabled ? "true" : "false",
    cfg.day, cfg.hour, cfg.minute, cfg.duration_min,
    cfg.load_transfer ? "true" : "false",
    state_str, remain,
    self->get_exercise_last_run().c_str(),
    self->get_exercise_fail_reason().c_str());

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json, strlen(json));
}

esp_err_t SmartgenHSC941Web::handle_api_exercise_post_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

  int content_len = req->content_len;
  if (content_len <= 0 || content_len > 512) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid request body");
    return ESP_FAIL;
  }

  char body[513];
  int received = httpd_req_recv(req, body, content_len);
  if (received <= 0) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Failed to read body");
    return ESP_FAIL;
  }
  body[received] = '\0';

  // Check for "action":"stop" to abort a running exercise
  if (strstr(body, "\"action\"") != nullptr && strstr(body, "\"stop\"") != nullptr) {
    self->stop_exercise_();
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    const char *r = R"({"ok":true,"msg":"Exercise stopped"})";
    return httpd_resp_send(req, r, strlen(r));
  }

  // Check for "action":"start" to manually trigger exercise now
  if (strstr(body, "\"action\"") != nullptr && strstr(body, "\"start\"") != nullptr) {
    if (self->get_exercise_state() != ExerciseState::IDLE) {
      httpd_resp_set_type(req, "application/json");
      httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
      const char *r = R"({"ok":false,"error":"Exercise already running"})";
      return httpd_resp_send(req, r, strlen(r));
    }
    self->start_exercise_();
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    const char *r = R"({"ok":true,"msg":"Exercise started"})";
    return httpd_resp_send(req, r, strlen(r));
  }

  // Otherwise parse config update
  auto find_int = [&](const char *key) -> int {
    const char *p = strstr(body, key);
    if (!p) return -1;
    p = strchr(p, ':');
    if (!p) return -1;
    return atoi(p + 1);
  };

  self->exercise_cfg_.enabled = (strstr(body, "\"enabled\":true") != nullptr);
  int v;
  if ((v = find_int("\"day\"")) >= 0 && v <= 7) self->exercise_cfg_.day = v;
  if ((v = find_int("\"hour\"")) >= 0 && v < 24) self->exercise_cfg_.hour = v;
  if ((v = find_int("\"minute\"")) >= 0 && v < 60) self->exercise_cfg_.minute = v;
  if ((v = find_int("\"duration\"")) >= 0 && v > 0 && v <= 120) self->exercise_cfg_.duration_min = v;
  self->exercise_cfg_.load_transfer = (strstr(body, "\"load_transfer\":true") != nullptr);

  self->save_exercise_config_();
  self->log_event(self->exercise_cfg_.enabled
    ? "Exercise config saved (enabled)"
    : "Exercise config saved (disabled)");

  // Reset trigger guard so schedule re-evaluates
  self->exercise_triggered_today_ = false;

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  const char *r = R"({"ok":true,"msg":"Config saved"})";
  return httpd_resp_send(req, r, strlen(r));
}

// ============================================================
//  Thermostat API handlers
// ============================================================

esp_err_t SmartgenHSC941Web::handle_api_thermostat_get_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

  const auto &thermostats = self->get_thermostats();
  const auto &relays = self->get_relays();

  std::string json;
  json.reserve(1024);
  json += "{\"relays\":[";
  bool first = true;
  for (uint8_t i = 0; i < MAX_RELAYS; i++) {
    if (relays[i].sw == nullptr) continue;
    if (!first) json += ',';
    first = false;
    json += "{\"idx\":";
    json += std::to_string(i);
    json += ",\"name\":\"";
    for (char c : relays[i].name) {
      if (c == '"') json += "\\\"";
      else if (c == '\\') json += "\\\\";
      else json += c;
    }
    json += "\",\"enabled\":";
    json += thermostats[i].enabled ? "true" : "false";
    json += ",\"sensor\":\"";
    json += thermostats[i].sensor_key;
    json += "\",\"on_below\":";
    char fbuf[16];
    snprintf(fbuf, sizeof(fbuf), "%.1f", thermostats[i].on_below);
    json += fbuf;
    json += ",\"off_above\":";
    snprintf(fbuf, sizeof(fbuf), "%.1f", thermostats[i].off_above);
    json += fbuf;
    // Include current sensor reading and relay state
    json += ",\"reading\":";
    if (!thermostats[i].sensor_key.empty()) {
      float v = self->get_sensor_value_by_key_(thermostats[i].sensor_key);
      if (std::isnan(v)) {
        json += "null";
      } else {
        snprintf(fbuf, sizeof(fbuf), "%.1f", v);
        json += fbuf;
      }
    } else {
      json += "null";
    }
    json += ",\"relay_on\":";
    json += relays[i].sw->state ? "true" : "false";
    json += '}';
  }
  json += "],\"sensors\":[";
  // Available sensor options
  json += "{\"key\":\"water_temp\",\"label\":\"Water Temp\"}";
  json += ",{\"key\":\"aux1\",\"label\":\"Aux Sensor 1\"}";
  if (self->get_ambient_temp() != nullptr) {
    json += ",{\"key\":\"ambient_temp\",\"label\":\"Ambient Temp\"}";
  }
  json += "]}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json.c_str(), json.size());
}

esp_err_t SmartgenHSC941Web::handle_api_thermostat_post_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

  int content_len = req->content_len;
  if (content_len <= 0 || content_len > 2048) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid request body");
    return ESP_FAIL;
  }

  char *body = new char[content_len + 1];
  int received = httpd_req_recv(req, body, content_len);
  if (received <= 0) {
    delete[] body;
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Failed to read body");
    return ESP_FAIL;
  }
  body[received] = '\0';

  // Parse array of relay thermostat configs:
  // [{"idx":0,"enabled":true,"sensor":"water_temp","on_below":5.0,"off_above":10.0}, ...]
  const char *p = body;
  while (true) {
    const char *start = strchr(p, '{');
    if (!start) break;
    const char *end = strchr(start, '}');
    if (!end) break;

    // Extract idx
    const char *idx_p = strstr(start, "\"idx\":");
    if (!idx_p || idx_p >= end) { p = end + 1; continue; }
    int idx = atoi(idx_p + 6);
    if (idx < 0 || idx >= MAX_RELAYS) { p = end + 1; continue; }

    // enabled
    self->relay_thermostats_[idx].enabled =
        (strstr(start, "\"enabled\":true") != nullptr);

    // sensor key
    const char *sk = strstr(start, "\"sensor\":\"");
    if (sk && sk < end) {
      sk += 10;
      const char *sq = strchr(sk, '"');
      if (sq && sq <= end) {
        self->relay_thermostats_[idx].sensor_key = std::string(sk, sq - sk);
      }
    } else {
      self->relay_thermostats_[idx].sensor_key = "";
    }

    // on_below
    const char *onp = strstr(start, "\"on_below\":");
    if (onp && onp < end) {
      self->relay_thermostats_[idx].on_below = strtof(onp + 11, nullptr);
    }

    // off_above
    const char *offp = strstr(start, "\"off_above\":");
    if (offp && offp < end) {
      self->relay_thermostats_[idx].off_above = strtof(offp + 12, nullptr);
    }

    p = end + 1;
  }

  delete[] body;
  self->save_thermostat_config_();
  self->log_event("Thermostat config saved");

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  const char *r = R"({"ok":true,"msg":"Thermostat config saved"})";
  return httpd_resp_send(req, r, strlen(r));
}

// ============================================================
//  Event log API handlers
// ============================================================

esp_err_t SmartgenHSC941Web::handle_api_eventlog_get_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

  const auto &log = self->get_event_log();

  // Build JSON: {"events":[{"ts":123456,"time":"2026-03-02 18:30","msg":"..."},...]
  // Newest first. Use chunked encoding to avoid huge single buffer.
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  httpd_resp_sendstr_chunk(req, "{\"events\":[");

  char buf[256];
  bool first = true;
  // Iterate in reverse (newest first)
  for (int i = static_cast<int>(log.size()) - 1; i >= 0; i--) {
    const auto &e = log[i];
    struct tm *tm_info = localtime(&e.timestamp);
    char time_str[20] = "?";
    if (tm_info) {
      strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    }

    // Escape any quotes in message
    std::string escaped_msg;
    escaped_msg.reserve(e.message.size());
    for (char c : e.message) {
      if (c == '"') escaped_msg += "\\\"";
      else if (c == '\\') escaped_msg += "\\\\";
      else escaped_msg += c;
    }

    int len = snprintf(buf, sizeof(buf),
      "%s{\"ts\":%ld,\"time\":\"%s\",\"msg\":\"%s\"}",
      first ? "" : ",",
      (long)e.timestamp, time_str, escaped_msg.c_str());
    first = false;

    if (len > 0 && len < (int)sizeof(buf)) {
      httpd_resp_sendstr_chunk(req, buf);
    }
  }

  httpd_resp_sendstr_chunk(req, "]}");
  httpd_resp_sendstr_chunk(req, nullptr);  // End chunked
  return ESP_OK;
}

esp_err_t SmartgenHSC941Web::handle_api_eventlog_post_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

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

  // Parse action: {"action":"clear"}
  if (strstr(body, "\"clear\"")) {
    self->event_log_.clear();
    self->clear_event_log_file_();
    self->log_event("Event log cleared");
    ESP_LOGI(TAG, "Event log cleared by user");
  }

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  const char *r = R"({"ok":true})";
  return httpd_resp_send(req, r, strlen(r));
}

}  // namespace smartgen_hsc941_web
}  // namespace esphome
