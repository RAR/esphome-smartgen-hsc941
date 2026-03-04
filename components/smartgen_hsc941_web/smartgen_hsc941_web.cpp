#include "smartgen_hsc941_web.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include <cstring>
#include <ctime>
#include <sys/stat.h>
#include <driver/ledc.h>

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
<meta name="theme-color" content="#0c0e14">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
<link rel="manifest" href="/manifest.json">
<title>HSC941 Genset Controller</title>
<style>
*,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
:root{
 --bg:#0c0e14;--surface:#141720;--card:#1a1e2b;--border:#252a3a;
 --text:#dee2ec;--dim:#6b7394;--faint:#3a4060;
 --red:#ef4444;--orange:#f59e0b;--green:#22c55e;--blue:#3b82f6;--cyan:#06b6d4;--purple:#a855f7;
 --red-bg:#ef444418;--orange-bg:#f59e0b18;--green-bg:#22c55e18;--blue-bg:#3b82f618;
 --gauge-track:#1e2235;--gauge-card:#1f2436;--gauge-border:#2d3350;--radius:10px;
}
html{font-size:16px}
body{font-family:'Inter',system-ui,-apple-system,sans-serif;background:var(--bg);color:var(--text);min-height:100vh;overflow-x:hidden}
@media(max-width:768px){html{font-size:15px}}

/* ── Header ── */
.hdr{background:var(--surface);border-bottom:1px solid var(--border);padding:10px 20px;display:flex;align-items:center;justify-content:space-between;position:sticky;top:0;z-index:50;backdrop-filter:blur(12px);gap:8px}
.hdr-left{display:flex;align-items:center;gap:10px;min-width:0}
.hdr-logo{width:32px;height:32px;background:linear-gradient(135deg,var(--blue),var(--cyan));border-radius:8px;display:flex;align-items:center;justify-content:center;flex-shrink:0}
.hdr-logo svg{width:18px;height:18px;fill:#fff}
.hdr-title{font-size:1rem;font-weight:700;letter-spacing:-.02em;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.hdr-sub{font-size:.65rem;color:var(--dim);font-weight:500;letter-spacing:.04em;text-transform:uppercase}
.hdr-right{display:flex;align-items:center;gap:6px;flex-shrink:0}
.hdr-sep{width:1px;height:18px;background:var(--border);margin:0 2px;flex-shrink:0}
.badge-ok{background:#16a34a;color:#fff}
.badge-warn{background:#d97706;color:#fff;animation:pulse-warn 1.5s infinite}
.badge-sd{background:#dc2626;color:#fff;animation:pulse-sd 1s infinite}
@keyframes pulse-warn{0%,100%{opacity:1}50%{opacity:.6}}
@keyframes pulse-sd{0%,100%{opacity:1}50%{opacity:.5}}
/* ── Nav Tabs ── */
.nav{display:flex;gap:0;background:var(--surface);border-bottom:1px solid var(--border);padding:0 16px;position:sticky;top:52px;z-index:49;align-items:center;justify-content:space-between;flex-wrap:wrap}
.nav-tabs{display:flex;gap:0}
.nav-tab{padding:10px 16px;font-size:.75rem;font-weight:600;letter-spacing:.03em;text-transform:uppercase;color:var(--dim);cursor:pointer;border-bottom:2px solid transparent;transition:color .2s,border-color .2s;user-select:none;background:none;border-top:none;border-left:none;border-right:none;white-space:nowrap}
.nav-tab:hover{color:var(--text)}
.nav-tab.active{color:var(--blue);border-bottom-color:var(--blue)}
.env-bar{display:flex;gap:12px;align-items:center;margin-left:auto;padding:4px 4px 4px 0}
.env-item{display:flex;align-items:center;gap:4px;font-size:.72rem;color:var(--dim)}
.env-val{font-weight:700;color:var(--text);font-size:.8rem}
.env-unit{font-size:.65rem;color:var(--dim)}
.section{display:none}
.section.active{display:block}
.badge{padding:3px 8px;border-radius:20px;font-size:.62rem;font-weight:700;letter-spacing:.04em;text-transform:uppercase;white-space:nowrap}
.badge-conn{background:var(--green-bg);color:var(--green);border:1px solid #22c55e30}
.badge-disc{background:var(--red-bg);color:var(--red);border:1px solid #ef444430;animation:pulse-red 2s infinite}
.badge-mode{background:var(--blue-bg);color:var(--blue);border:1px solid #3b82f630}
.badge-auto{background:var(--green-bg);color:var(--green);border:1px solid #22c55e30}
.badge-manual{background:#f59e0b18;color:var(--orange);border:1px solid #f59e0b30}
.unit-toggle{background:var(--card);border:1px solid var(--border);color:var(--dim);padding:3px 8px;border-radius:20px;font-size:.62rem;font-weight:700;letter-spacing:.04em;cursor:pointer;transition:color .2s,border-color .2s;user-select:none}
.unit-toggle:hover{color:var(--text);border-color:var(--blue)}
.badge-stop{background:var(--red-bg);color:var(--red);border:1px solid #ef444430}
@keyframes pulse-red{0%,100%{opacity:1}50%{opacity:.6}}
/* ── Mobile: Header + Nav ── */
@media(max-width:600px){
.hdr{padding:8px 12px}
.hdr-logo{width:28px;height:28px;border-radius:6px}.hdr-logo svg{width:16px;height:16px}
.hdr-title{font-size:.85rem}
.hdr-sub{display:none}
.hdr-right{gap:4px}
.hdr-sep{height:14px;margin:0 1px}
.badge{padding:2px 6px;font-size:.56rem}
.unit-toggle{padding:2px 6px;font-size:.56rem}
.nav{top:44px;padding:0 8px}
.nav-tab{padding:8px 10px;font-size:.68rem}
.env-bar{gap:8px;font-size:.65rem}
.env-item{font-size:.65rem}
.env-val{font-size:.72rem}
}
@media(max-width:380px){
.hdr-title{font-size:.78rem}
.hdr-logo{width:24px;height:24px}.hdr-logo svg{width:14px;height:14px}
.hdr-right{gap:3px}
.badge{padding:2px 5px;font-size:.52rem}
.unit-toggle{padding:2px 5px;font-size:.52rem}
.nav{top:40px}
.nav-tab{padding:7px 8px;font-size:.62rem}
.env-bar{display:none}
}

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
.r-gauges .card{background:var(--gauge-card);border-color:var(--gauge-border)}
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

/* ── Sparkline ── */
.sparkline{width:100%;height:32px;display:block;margin-top:4px}
.spark-line{fill:none;stroke-width:1.5;stroke-linecap:round;stroke-linejoin:round}
.spark-area{opacity:.15}

/* ── Battery trend ── */
.batt-card-body{padding:12px 14px}
.batt-hdr{display:flex;justify-content:space-between;align-items:center;margin-bottom:6px}
.batt-stats{display:flex;gap:14px}
.batt-stat{text-align:center}
.batt-stat-val{font-size:.95rem;font-weight:700;font-variant-numeric:tabular-nums}
.batt-stat-lbl{font-size:.52rem;color:var(--dim);text-transform:uppercase;letter-spacing:.05em;font-weight:600}
.batt-now{font-size:1.4rem;font-weight:700;font-variant-numeric:tabular-nums}
.batt-now-lbl{font-size:.52rem;color:var(--dim);text-transform:uppercase;letter-spacing:.05em;font-weight:600}
.batt-spark-wrap{width:100%;height:48px;position:relative}
.batt-spark-wrap svg{display:block;width:100%;height:100%}
@media(max-width:600px){.batt-stats{gap:10px}.batt-stat-val{font-size:.82rem}.batt-now{font-size:1.1rem}.batt-spark-wrap{height:40px}}


/* ── Maintenance ── */
.maint-item{display:flex;justify-content:space-between;align-items:center;padding:10px 12px;background:var(--surface);border-radius:8px;margin-bottom:6px;border:1px solid var(--border)}
.maint-item.maint-due{border-color:var(--orange);background:#f59e0b08}
.maint-item.maint-overdue{border-color:var(--red);background:#ef444408}
.maint-name{font-weight:600;font-size:.82rem}
.maint-info{font-size:.72rem;color:var(--dim)}
.maint-remaining{font-weight:700;font-size:.82rem}
.maint-remaining.due{color:var(--orange)}
.maint-remaining.overdue{color:var(--red)}
.maint-remaining.ok{color:var(--green)}
.maint-fields{display:grid;grid-template-columns:1fr 1fr 1fr;gap:8px;margin-top:8px}
@media(max-width:600px){.maint-fields{grid-template-columns:1fr}}
.maint-btns{display:flex;gap:8px;margin-top:10px;justify-content:flex-end}
.maint-edit-row{display:flex;align-items:center;gap:4px;margin-top:2px}
.maint-edit-row input{width:60px;padding:2px 4px;background:var(--bg);color:var(--text);border:1px solid var(--border);border-radius:4px;font-size:.72rem;text-align:center}
.maint-edit-btn{background:none;border:none;color:var(--dim);cursor:pointer;padding:1px 4px;font-size:.72rem}
.maint-edit-btn:hover{color:var(--cyan)}
.maint-toolbar{display:flex;gap:8px;margin-bottom:10px;justify-content:flex-end}
.maint-toolbar button{font-size:.68rem;padding:4px 10px;border-radius:4px;border:1px solid var(--border);background:var(--surface);color:var(--text);cursor:pointer}
.maint-toolbar button:hover{border-color:var(--cyan);color:var(--cyan)}
.maint-toolbar input[type=file]{display:none}

/* ── Fuel Estimator ── */
.fuel-bar-outer{height:20px;background:var(--surface);border-radius:10px;overflow:hidden;border:1px solid var(--border);position:relative}
.fuel-bar-inner{height:100%;border-radius:10px;transition:width .5s}
.fuel-bar-text{position:absolute;inset:0;display:flex;align-items:center;justify-content:center;font-size:.7rem;font-weight:700}
.fuel-stats{display:flex;gap:16px;margin-top:8px;flex-wrap:wrap}
.fuel-stat{font-size:.72rem;color:var(--dim)}
.fuel-stat span{font-weight:700;color:var(--text)}

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
/* ── Light Theme ── */
[data-theme="light"]{--bg:#f0f2f5;--surface:#f8f9fb;--card:#fff;--border:#d0d5dd;--text:#111827;--dim:#4b5563;--faint:#9ca3af;--gauge-track:#c0c7d0;--gauge-card:#edf0f4;--gauge-border:#d0d5dd;--red-bg:#fef2f2;--orange-bg:#fffbeb;--green-bg:#f0fdf4;--blue-bg:#eff6ff}
[data-theme="light"] .hdr,[data-theme="light"] .nav{background:#fff;border-color:#d0d5dd}
[data-theme="light"] .card{box-shadow:0 1px 3px rgba(0,0,0,.1)}
[data-theme="light"] .ex-field select,[data-theme="light"] .ex-field input,[data-theme="light"] .th-field select,[data-theme="light"] .th-field input{background:#f3f4f6}
[data-theme="light"] .relay-item{background:#f8f9fb}
[data-theme="light"] ::-webkit-scrollbar-thumb{background:#c0c4cc}
/* ── Header Icon Buttons ── */
.hdr-btn{background:var(--card);border:1px solid var(--border);border-radius:20px;color:var(--dim);cursor:pointer;font-size:.72rem;padding:3px 8px;line-height:1;transition:all .15s;display:flex;align-items:center;justify-content:center;height:22px;min-width:22px}.hdr-btn:hover{color:var(--text);border-color:var(--text)}
.hdr-btn svg{width:12px;height:12px}
@media(max-width:600px){.hdr-btn{height:20px;min-width:20px;padding:2px 6px;font-size:.65rem}.hdr-btn svg{width:11px;height:11px}}
@media(max-width:380px){.hdr-btn{height:18px;min-width:18px;padding:2px 5px}.hdr-btn svg{width:10px;height:10px}}
/* ── PIN Lock Overlay ── */
.pin-ov{position:fixed;inset:0;background:rgba(0,0,0,.7);z-index:200;display:flex;align-items:center;justify-content:center;opacity:0;pointer-events:none;transition:opacity .2s}.pin-ov.show{opacity:1;pointer-events:auto}
.pin-box{background:var(--card);border:1px solid var(--border);border-radius:14px;padding:24px 20px;width:280px;text-align:center}
.pin-box h3{margin:0 0 4px;font-size:.95rem}.pin-sub{font-size:.7rem;color:var(--dim);margin-bottom:14px}
.pin-dg{display:flex;gap:8px;justify-content:center;margin-bottom:14px}
.pin-dg input{width:42px;height:50px;background:var(--surface);border:2px solid var(--border);border-radius:8px;color:var(--text);font-size:1.3rem;text-align:center;font-weight:700;outline:none;-webkit-text-security:disc}
.pin-dg input:focus{border-color:var(--blue)}
.pin-btns{display:flex;gap:8px;justify-content:center}.pin-btns button{flex:1;padding:8px 0;border-radius:6px;font-size:.76rem;font-weight:700;cursor:pointer;border:none;transition:all .15s}
.pin-ok{background:var(--blue);color:#fff}.pin-cn{background:var(--faint);color:var(--text)}
.pin-msg{font-size:.7rem;color:var(--red);margin-top:8px;min-height:16px}

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
  <button class="hdr-btn" id="themeBtn" onclick="toggleTheme()" title="Toggle theme">&#9790;</button>
  <button class="hdr-btn" id="lockBtn" onclick="openPinModal()" title="PIN lock"><svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2"/><path d="M7 11V7a5 5 0 0110 0v4"/></svg></button>
  <button class="unit-toggle" id="unitToggle" onclick="toggleUnit()">&deg;C</button>
  <div class="hdr-sep"></div>
  <span class="badge badge-ok" id="alarmBadge">OK</span>
  <span class="badge badge-disc" id="connBadge">OFFLINE</span>
  <span class="badge badge-mode" id="modeBadge">---</span>
 </div>
</header>

<nav class="nav">
 <div class="nav-tabs">
  <button class="nav-tab active" data-sec="monitor" onclick="switchSec('monitor')">Monitoring</button>
  <button class="nav-tab" data-sec="config" onclick="switchSec('config')">Configuration</button>
  <button class="nav-tab" data-sec="history" onclick="switchSec('history')">History</button>
 </div>
 <div class="env-bar" id="envBar">
  <div class="env-item" id="ambItem" style="display:none"><span id="lblAmb">Ambient Temp</span>&nbsp;<span class="env-val" id="v_amb">--</span><span class="env-unit tunit">&#176;C</span></div>
  <div class="env-item" id="humItem" style="display:none"><span id="lblHum">Humidity</span>&nbsp;<span class="env-val" id="v_hum">--</span><span class="env-unit">%</span></div>
  <div class="env-item" id="mainsItem" style="display:none"><span>Mains</span>&nbsp;<span class="env-val" id="v_mains" style="color:var(--green)">OK</span></div>
 </div>
</nav>

<main class="main">

<div class="section active" id="sec-monitor">
<!-- Gauges Row -->
<div class="row r-gauges">
 <div class="card"><div class="card-body gauge-wrap" id="gVolt"></div></div>
 <div class="card"><div class="card-body gauge-wrap" id="gFreq"></div></div>
 <div class="card"><div class="card-body gauge-wrap" id="gRPM"></div></div>
 <div class="card"><div class="card-body gauge-wrap" id="gLoad"><div id="loadSparkWrap"></div></div></div>
</div>

<!-- 3-Phase Electrical + Engine -->
<div class="row r-dual">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg><h2>Generator Output</h2></div>
  <div class="card-body">
   <div class="phases">
    <div class="phase ph-a" id="phA"><div class="phase-label" id="lblPhA">Phase A</div><div class="phase-val" id="pv_a">--</div><div class="phase-sub"><span id="pi_a">--</span> A &bull; <span id="pp_a">--</span> kW</div></div>
    <div class="phase ph-b" id="phB"><div class="phase-label" id="lblPhB">Phase B</div><div class="phase-val" id="pv_b">--</div><div class="phase-sub"><span id="pi_b">--</span> A &bull; <span id="pp_b">--</span> kW</div></div>
    <div class="phase ph-c" id="phC"><div class="phase-label" id="lblPhC">Phase C</div><div class="phase-val" id="pv_c">--</div><div class="phase-sub"><span id="pi_c">--</span> A &bull; <span id="pp_c">--</span> kW</div></div>
   </div>
   <table class="dtable" style="margin-top:10px">
    <tr id="llAB"><td class="dlbl" id="lblLLAB">Line-Line AB</td><td class="dval"><span id="v_ab">--</span><span class="dunit">V</span></td></tr>
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
    <tr><td class="dlbl">Water Temperature</td><td class="dval"><span id="v_wt">--</span><span class="dunit tunit">&#176;C</span></td></tr>
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

<!-- ATS Power Flow -->
<!-- I/O -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="2" y="3" width="20" height="18" rx="2"/><path d="M8 7h8m-8 4h8m-8 4h5"/></svg><h2>Inputs / Outputs</h2></div>
  <div class="card-body"><div class="io-grid" id="ioPanel"></div></div>
 </div>
</div>
<!-- Fuel Level -->
<div class="row r-full" id="fuelSection" style="display:none">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M3 22V5a2 2 0 012-2h6a2 2 0 012 2v17"/><path d="M13 10h4a2 2 0 012 2v10"/></svg><h2 id="fuelTitle">Fuel Level</h2></div>
  <div class="card-body">
   <div class="fuel-bar-outer"><div class="fuel-bar-inner" id="fuelBar"></div><div class="fuel-bar-text" id="fuelPct">--</div></div>
   <div class="fuel-stats" id="fuelStats">
    <div class="fuel-stat" id="fuelRemainStat">Remaining: <span id="fuelRemain">--</span> <span id="fuelVolUnit">L</span></div>
    <div class="fuel-stat" id="fuelRateStat" style="display:none">Burn Rate: <span id="fuelRate">--</span> <span id="fuelRateUnit">lph</span></div>
    <div class="fuel-stat" id="fuelRuntimeStat">Est. Runtime: <span id="fuelRuntime">--</span> h</div>
    <div class="fuel-stat" id="fuelTankStat">Tank: <span id="fuelTank">--</span> <span id="fuelTankUnit">L</span></div>
    <button class="evtlog-clear" style="margin-left:auto" id="fuelFillBtn" onclick="fillTank()">Mark Filled</button>
   </div>
  </div>
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
     <span class="ex-st-label" style="margin-left:16px" id="exNextLabel">Next:</span><span class="ex-st-value" id="exNext">--</span>
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
<!-- Maintenance Tracker -->
<div class="row r-full" id="maintSection" style="display:none">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M14.7 6.3a1 1 0 000 1.4l1.6 1.6a1 1 0 001.4 0l3.77-3.77a6 6 0 01-7.94 7.94l-6.91 6.91a2.12 2.12 0 01-3-3l6.91-6.91a6 6 0 017.94-7.94l-3.76 3.76z"/></svg><h2>Maintenance Tracker</h2></div>
  <div class="card-body">
   <div id="maintList"></div>
  </div>
 </div>
</div>
<!-- Config Export / Import -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M21 15v4a2 2 0 01-2 2H5a2 2 0 01-2-2v-4"/><polyline points="7 10 12 15 17 10"/><line x1="12" y1="15" x2="12" y2="3"/></svg><h2>Backup &amp; Restore</h2></div>
  <div class="card-body">
   <p style="font-size:.72rem;color:var(--dim);margin:0 0 10px">Export or import all configuration: exercise schedule, thermostat, maintenance tracker, and fuel settings.</p>
   <div class="maint-toolbar">
    <button onclick="exportAllConfig()">Export All Config</button>
    <button onclick="document.getElementById('cfgFileIn').click()">Import All Config</button>
    <input type="file" id="cfgFileIn" accept=".json" onchange="importAllConfig(event)">
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
<!-- Battery Voltage -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="1" y="6" width="18" height="12" rx="2"/><line x1="23" y1="10" x2="23" y2="14"/></svg><h2>Battery Voltage</h2></div>
  <div class="batt-card-body">
   <div class="batt-hdr">
    <div><div class="batt-now" id="battNow">--</div><div class="batt-now-lbl">Current</div></div>
    <div class="batt-stats">
     <div class="batt-stat"><div class="batt-stat-val" style="color:var(--cyan)" id="battMin">--</div><div class="batt-stat-lbl">Min</div></div>
     <div class="batt-stat"><div class="batt-stat-val" style="color:var(--dim)" id="battAvg">--</div><div class="batt-stat-lbl">Avg</div></div>
     <div class="batt-stat"><div class="batt-stat-val" style="color:var(--orange)" id="battMax">--</div><div class="batt-stat-lbl">Max</div></div>
    </div>
   </div>
   <div class="batt-spark-wrap" id="battSparkWrap"></div>
  </div>
 </div>
</div>
<!-- Runtime History -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M18 20V10"/><path d="M12 20V4"/><path d="M6 20v-6"/></svg><h2>Runtime History</h2></div>
  <div class="card-body"><svg id="runtimeChart" width="100%" height="140" style="display:block"></svg></div>
 </div>
</div>
<!-- Event Log -->
<div class="row r-full">
 <div class="card">
  <div class="card-hd"><svg class="ico" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/></svg><h2>Event Log</h2></div>
  <div class="card-body">
   <div class="evtlog-bar">
    <span class="evtlog-count" id="evtCount"></span>
    <div style="display:flex;gap:8px">
     <button class="evtlog-clear" onclick="exportCSV()">Export CSV</button>
     <button class="evtlog-clear" onclick="clearEventLog()">Clear Log</button>
    </div>
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

<!-- PIN Lock Modal -->
<div class="pin-ov" id="pinOverlay">
 <div class="pin-box">
  <h3 id="pinTitle">Enter PIN</h3>
  <div class="pin-sub" id="pinSub">PIN required for control actions</div>
  <div class="pin-dg">
   <input type="tel" maxlength="1" id="pin0" oninput="pinAdv(this,1)" onkeydown="pinBk(event,0)">
   <input type="tel" maxlength="1" id="pin1" oninput="pinAdv(this,2)" onkeydown="pinBk(event,1)">
   <input type="tel" maxlength="1" id="pin2" oninput="pinAdv(this,3)" onkeydown="pinBk(event,2)">
   <input type="tel" maxlength="1" id="pin3" oninput="pinAdv(this,null)" onkeydown="pinBk(event,3)">
  </div>
  <div class="pin-btns">
   <button class="pin-cn" onclick="closePinModal()">Cancel</button>
   <button class="pin-ok" id="pinOkBtn" onclick="pinOk()">Unlock</button>
  </div>
  <div class="pin-msg" id="pinMsg"></div>
 </div>
</div>

<!-- Toast -->
<div class="toast" id="toast"></div>

<script>
/* ── Theme toggle ── */
let darkTheme=localStorage.getItem('theme')!=='light';
function applyThemeMode(){document.documentElement.setAttribute('data-theme',darkTheme?'dark':'light');const b=document.getElementById('themeBtn');if(b)b.innerHTML=darkTheme?'&#9790;':'&#9728;';const m=document.querySelector('meta[name="theme-color"]');if(m)m.content=darkTheme?'#0c0e14':'#f0f2f5';}
function toggleTheme(){darkTheme=!darkTheme;localStorage.setItem('theme',darkTheme?'dark':'light');applyThemeMode();}
applyThemeMode();
/* ── Temperature unit toggle ── */
let useFahr=localStorage.getItem('tempUnit')==='F';
function cToF(c){return c*9/5+32;}
function tempVal(c,d){if(c==null)return'--';const v=useFahr?cToF(c):c;return v.toFixed(d!=null?d:1);}
function tempUnit(){return useFahr?'\u00b0F':'\u00b0C';}
function toggleUnit(){useFahr=!useFahr;localStorage.setItem('tempUnit',useFahr?'F':'C');document.getElementById('unitToggle').textContent=useFahr?'\u00b0F':'\u00b0C';updateTempUnits();if(lastData)update(lastData);if(thData&&thData.length)renderThermostat();}
function updateTempUnits(){document.querySelectorAll('.tunit').forEach(e=>{e.innerHTML=useFahr?'&deg;F':'&deg;C';});}
let lastData=null;
{const b=document.getElementById('unitToggle');if(b)b.textContent=useFahr?'\u00b0F':'\u00b0C';}

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
 lastData=d;
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
 const sp=d.single_phase||false;
 const hasC=!sp&&s.gen_vc!=null&&s.gen_vc>0;
 const hide=(id,show)=>{const e=document.getElementById(id);if(e)e.style.display=show?'':'none';};
 hide('phC',hasC);
 const pg=document.querySelector('.phases');if(pg)pg.style.gridTemplateColumns=hasC?'1fr 1fr 1fr':'1fr 1fr';
 hide('llBC',hasC);hide('llCA',hasC);
 // Rename labels for single-phase / split-phase
 if(sp){
  const la=document.getElementById('lblPhA');if(la)la.textContent='L1';
  const lb=document.getElementById('lblPhB');if(lb)lb.textContent='L2';
  const ll=document.getElementById('lblLLAB');if(ll)ll.textContent='L1 \u2013 L2';
 }
 // Line-line + power
 const t=[['v_ab','gen_vab'],['v_bc','gen_vbc'],['v_ca','gen_vca'],['v_tkw','total_kw'],['v_kvar','kvar'],['v_kva','kva'],['v_pf','pf']];
 t.forEach(([el,k])=>{const e=document.getElementById(el);if(e)e.textContent=f(s[k]);});
 // Engine
 const ENG_STATES=['Standby','Preheat','Fuel Output','Crank','Crank Rest','Safety On','Start Idle','Warming Up','Wait On-load','Running','Cooling','Stop Idle','ETS','Wait Stop','Stop Fail'];
 const AUTO_STATES=['Standby','Mains OK Delay','Mains Fail Delay','Start Delay','Starting','Warm-up','ATS Close Delay','On Load','Mains Return Delay','Cool Down','ATS Open Delay','ATS Opened','Stop Delay','Stopping'];
 const eng=[['v_op','oil_press'],['v_bv','batt_v'],['v_cv','charge_v'],['v_rpm2','rpm'],['v_aux','aux1']];
 eng.forEach(([el,k])=>{const e=document.getElementById(el);if(e)e.textContent=f(s[k]);});
 {const e=document.getElementById('v_es');if(e){const v=s.eng_status;e.textContent=v!=null?(ENG_STATES[Math.floor(v)]||v):'--';}}
 {const e=document.getElementById('v_as');if(e){const v=s.auto_status;e.textContent=v!=null?(AUTO_STATES[Math.floor(v)]||v):'--';}}
 // Water temp with unit conversion
 {const e=document.getElementById('v_wt');if(e)e.textContent=tempVal(s.water_temp,1);}
 pushSpark(s.load_pct);renderSparkline();pushBatt(s.batt_v);
 if(d.mains_ok!=null){const mi=document.getElementById('mainsItem');if(mi)mi.style.display='';const mv=document.getElementById('v_mains');if(mv){mv.textContent=d.mains_ok?'OK':'FAIL';mv.style.color=d.mains_ok?'var(--green)':'var(--red)';}}
 if(d.language)applyI18n(d.language);
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
  const ai=document.getElementById('ambItem');if(ai)ai.style.display='';
  const ae=document.getElementById('v_amb');if(ae)ae.textContent=tempVal(d.ambient_temp,1);
  if(d.ambient_temp_name){const al=document.getElementById('lblAmb');if(al)al.textContent=d.ambient_temp_name;}
 }
 // Ambient humidity
 if(d.ambient_humidity!=null){
  const hi=document.getElementById('humItem');if(hi)hi.style.display='';
  const he=document.getElementById('v_hum');if(he)he.textContent=f(d.ambient_humidity,1);
  if(d.ambient_humidity_name){const hl=document.getElementById('lblHum');if(hl)hl.textContent=d.ambient_humidity_name;}
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
 if(requirePin(function(){confirmCmd(coil,title,msg,style);}))return;
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
 if(requirePin(function(){var c=document.getElementById('rly_cb_'+idx);if(c)c.checked=on;toggleRelay(idx,on);})){var c=document.getElementById('rly_cb_'+idx);if(c)c.checked=!on;return;}
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
 updateExCountdown(d);
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
  const rdTxt=r.reading!=null?tempVal(r.reading,1)+tempUnit():'--';
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
  var onDisp=useFahr?cToF(r.on_below):r.on_below;
  var offDisp=useFahr?cToF(r.off_above):r.off_above;
  h+='<div class="th-field"><label>ON Below ('+tempUnit()+')</label><input type="number" class="th-on" data-idx="'+r.idx+'" step="0.5" value="'+onDisp.toFixed(1)+'"></div>';
  h+='<div class="th-field"><label>OFF Above ('+tempUnit()+')</label><input type="number" class="th-off" data-idx="'+r.idx+'" step="0.5" value="'+offDisp.toFixed(1)+'"></div>';
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
    rd.textContent=(r.reading!=null?tempVal(r.reading,1)+tempUnit():'--')+' ('+(r.relay_on?'ON':'OFF')+')';
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
   on_below:useFahr?(((el_on?parseFloat(el_on.value):5.0)-32)*5/9):(el_on?parseFloat(el_on.value):5.0),
   off_above:useFahr?(((el_off?parseFloat(el_off.value):10.0)-32)*5/9):(el_off?parseFloat(el_off.value):10.0)
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

/* ── PIN Lock ── */
var pinCode=typeof cfgPin!=='undefined'?cfgPin:'',pinEnabled=pinCode.length===4,pinVerified=false,pendingCmdAfterPin=null;
function requirePin(cb){if(!pinEnabled||pinVerified)return false;pendingCmdAfterPin=cb;document.getElementById('pinTitle').textContent='Enter PIN';document.getElementById('pinSub').textContent='PIN required for this action';document.getElementById('pinOkBtn').textContent='Unlock';clearPinInputs();document.getElementById('pinMsg').textContent='';document.getElementById('pinOverlay').classList.add('show');setTimeout(function(){document.getElementById('pin0').focus();},100);return true;}
function updateLockBtn(){var b=document.getElementById('lockBtn');if(!b)return;if(!pinEnabled){b.style.display='none';return;}b.innerHTML=pinVerified?'<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2"/><path d="M7 11V7a5 5 0 0119 0"/></svg>':'<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2"/><path d="M7 11V7a5 5 0 0110 0v4"/></svg>';b.title=pinVerified?'Controls unlocked (click to re-lock)':'PIN lock active';}
function openPinModal(){if(!pinEnabled)return;if(pinVerified){pinVerified=false;updateLockBtn();toast('Controls locked','ok');return;}document.getElementById('pinTitle').textContent='Unlock';document.getElementById('pinSub').textContent='Enter PIN to unlock controls';document.getElementById('pinOkBtn').textContent='Unlock';clearPinInputs();document.getElementById('pinMsg').textContent='';document.getElementById('pinOverlay').classList.add('show');setTimeout(function(){document.getElementById('pin0').focus();},100);}
function closePinModal(){document.getElementById('pinOverlay').classList.remove('show');}
function clearPinInputs(){for(var i=0;i<4;i++)document.getElementById('pin'+i).value='';}
function getPinVal(){var v='';for(var i=0;i<4;i++)v+=document.getElementById('pin'+i).value;return v;}
function pinAdv(el,nx){el.value=el.value.replace(/[^0-9]/g,'').slice(-1);if(el.value&&nx!=null)document.getElementById('pin'+nx).focus();}
function pinBk(ev,i){if(ev.key==='Backspace'&&!ev.target.value&&i>0){document.getElementById('pin'+(i-1)).focus();}}
function pinOk(){var v=getPinVal();if(v.length!==4){document.getElementById('pinMsg').textContent='Enter all 4 digits';return;}if(v===pinCode){pinVerified=true;updateLockBtn();closePinModal();toast('Controls unlocked','ok');if(pendingCmdAfterPin){pendingCmdAfterPin();pendingCmdAfterPin=null;}}else{document.getElementById('pinMsg').textContent='Wrong PIN';clearPinInputs();document.getElementById('pin0').focus();}}
updateLockBtn();



/* ── Init ── */
initGauges();initPanels();updateTempUnits();poll();setInterval(poll,2500);
loadExercise();setInterval(pollExercise,3000);
loadThermostat();setInterval(pollThermostat,5000);
loadEventLog();setInterval(loadEventLog,10000);
loadMaint();setInterval(loadMaint,30000);
loadFuel();setInterval(loadFuel,15000);
loadRuntimeHistory();
loadBattHistory();

function toggleAnnFull(){
 const f=document.getElementById('annFull');
 const t=document.getElementById('annToggle');
 f.classList.toggle('show');
 t.classList.toggle('open');
 t.querySelector('svg').nextSibling.textContent=f.classList.contains('show')?' Hide indicators':' Show all indicators';
}

/* ── CSV Export ── */
function exportCSV(){fetch('/api/eventlog').then(r=>r.json()).then(d=>{
 if(!d.events||!d.events.length){toast('No events to export','err');return;}
 let csv='Timestamp,Event\n';d.events.forEach(e=>{csv+='"'+e.time+'","'+e.msg.replace(/"/g,'""')+'"\n';});
 const b=new Blob([csv],{type:'text/csv'});const a=document.createElement('a');a.href=URL.createObjectURL(b);
 a.download='genset_events_'+new Date().toISOString().slice(0,10)+'.csv';a.click();URL.revokeObjectURL(a.href);toast('CSV exported','ok');
}).catch(()=>toast('Export failed','err'));}

/* ── Next Exercise Countdown ── */
function updateExCountdown(d){
 const el=document.getElementById('exNext');if(!el)return;
 if(!d||!d.enabled||d.state!=='idle'){el.textContent='--';return;}
 const now=new Date();let next=new Date();next.setHours(d.hour,d.minute,0,0);
 if(d.day<7){let diff=d.day-now.getDay();if(diff<0||(diff===0&&now>=next))diff+=7;next.setDate(now.getDate()+diff);}else{if(now>=next)next.setDate(next.getDate()+1);}
 const ms=next-now;if(ms<=0){el.textContent='Imminent';return;}
 const h=Math.floor(ms/3600000),m=Math.floor((ms%3600000)/60000);
 el.textContent=h>=24?Math.floor(h/24)+'d '+(h%24)+'h':h+'h '+m+'m';
}

/* ── Load History Sparkline ── */
const sparkBuf=[];
function pushSpark(v){sparkBuf.push(v!=null?v:0);if(sparkBuf.length>120)sparkBuf.shift();}
function renderSparkline(){const w=document.getElementById('loadSparkWrap');if(!w||sparkBuf.length<3)return;
 const W=w.clientWidth||160,H=32,mx=Math.max(...sparkBuf,1);
 const pts=sparkBuf.map((v,i)=>((i/(sparkBuf.length-1))*W).toFixed(1)+','+(H-v/mx*H).toFixed(1)).join(' ');
 w.innerHTML='<svg class="sparkline" viewBox="0 0 '+W+' '+H+'" preserveAspectRatio="none"><polygon class="spark-area" points="0,'+H+' '+pts+' '+W+','+H+'" fill="var(--green)"/><polyline class="spark-line" points="'+pts+'" stroke="var(--green)"/></svg>';
}

/* ── Battery Voltage Trend ── */
let bMin=999,bMax=0,bSum=0,bCnt=0;
const battBuf=[];
function pushBatt(v){if(v==null||v<=0)return;
 battBuf.push(v);if(battBuf.length>240)battBuf.shift();
 const mn=document.getElementById('battMin'),mx=document.getElementById('battMax'),av=document.getElementById('battAvg'),nw=document.getElementById('battNow');
 if(nw)nw.textContent=v.toFixed(1)+'V';
 if(bCnt>0){if(mn)mn.textContent=bMin.toFixed(1)+'V';if(mx)mx.textContent=bMax.toFixed(1)+'V';if(av)av.textContent=(bSum/bCnt).toFixed(1)+'V';}
 renderBattSparkline();
}
function loadBattHistory(){fetch('/api/battery_history').then(r=>r.json()).then(d=>{
 if(d.samples&&d.samples.length){d.samples.forEach(v=>battBuf.push(v));}
 if(d.count>0){bMin=d.min;bMax=d.max;bSum=d.avg*d.count;bCnt=d.count;
  const mn=document.getElementById('battMin'),mx=document.getElementById('battMax'),av=document.getElementById('battAvg');
  if(mn)mn.textContent=bMin.toFixed(1)+'V';if(mx)mx.textContent=bMax.toFixed(1)+'V';if(av)av.textContent=(bSum/bCnt).toFixed(1)+'V';
 }
 if(battBuf.length>0){const nw=document.getElementById('battNow');if(nw)nw.textContent=battBuf[battBuf.length-1].toFixed(1)+'V';}
 renderBattSparkline();
}).catch(()=>{});}
function renderBattSparkline(){const w=document.getElementById('battSparkWrap');if(!w||battBuf.length<3)return;
 const W=w.clientWidth||200,H=48;
 const lo=Math.min(...battBuf),hi=Math.max(...battBuf);
 const range=hi-lo||1;const pad=range*0.1;
 const yMin=lo-pad,yMax=hi+pad,yR=yMax-yMin;
 const pts=battBuf.map((v,i)=>((i/(battBuf.length-1))*W).toFixed(1)+','+(H-(v-yMin)/yR*H).toFixed(1)).join(' ');
 w.innerHTML='<svg viewBox="0 0 '+W+' '+H+'" preserveAspectRatio="none"><polygon points="0,'+H+' '+pts+' '+W+','+H+'" fill="var(--cyan)" opacity=".12"/><polyline points="'+pts+'" fill="none" stroke="var(--cyan)" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/></svg>';
}

/* ── Maintenance Tracker ── */
function loadMaint(){fetch('/api/maintenance').then(r=>r.json()).then(d=>{
 const s=document.getElementById('maintSection');
 if(!d.items||!d.items.length){if(s)s.style.display='none';return;}
 if(s)s.style.display='';renderMaint(d.items,d.total_hours||0);
}).catch(()=>{});}
function renderMaint(items,hrs){const el=document.getElementById('maintList');if(!el)return;let h='';
 items.forEach((m,i)=>{const used=hrs-m.reset;const rem=m.interval-used;
  const cls=rem<=0?'maint-overdue':rem<m.interval*0.1?'maint-due':'';
  const rc=rem<=0?'overdue':rem<m.interval*0.1?'due':'ok';
  h+='<div class="maint-item '+cls+'"><div><div class="maint-name">'+m.name+'</div>';
  h+='<div class="maint-edit-row"><span class="maint-info" id="maintIv'+i+'">Every '+m.interval+'h</span>';
  h+=' <button class="maint-edit-btn" title="Edit interval" onclick="editMaintInterval('+i+','+m.interval+')">\u270E</button></div>';
  h+='<div class="maint-info">'+Math.round(used)+'h since service</div></div>';
  h+='<div style="text-align:right"><div class="maint-remaining '+rc+'">'+(rem>0?Math.round(rem)+'h left':'OVERDUE '+Math.abs(Math.round(rem))+'h')+'</div>';
  h+='<button class="evtlog-clear" style="margin-top:4px" onclick="resetMaint('+i+')">Reset</button></div></div>';
 });el.innerHTML=h;}
function editMaintInterval(i,cur){const sp=document.getElementById('maintIv'+i);if(!sp)return;
 sp.innerHTML='<input type="number" min="1" max="9999" value="'+cur+'" id="maintIvIn'+i+'" onkeydown="if(event.key===\'Enter\')saveMaintInterval('+i+')">';
 sp.innerHTML+=' <button class="maint-edit-btn" onclick="saveMaintInterval('+i+')">\u2714</button>';}
function saveMaintInterval(i){const inp=document.getElementById('maintIvIn'+i);if(!inp)return;
 const v=parseInt(inp.value);if(!v||v<1){toast('Invalid interval','err');return;}
 fetch('/api/maintenance',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'update',index:i,interval:v})})
 .then(r=>r.json()).then(d=>{toast(d.ok?'Interval updated':'Failed',d.ok?'ok':'err');loadMaint();}).catch(()=>toast('Error','err'));}
function resetMaint(i){fetch('/api/maintenance',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'reset',index:i})})
 .then(r=>r.json()).then(d=>{toast(d.ok?'Service reset':'Failed',d.ok?'ok':'err');loadMaint();}).catch(()=>toast('Error','err'));}

/* ── Config Export / Import (all settings) ── */
async function exportAllConfig(){
 try{
  const [ex,th,mt,fu]=await Promise.all([
   fetch('/api/exercise').then(r=>r.json()),
   fetch('/api/thermostat').then(r=>r.json()),
   fetch('/api/maintenance').then(r=>r.json()),
   fetch('/api/fuel').then(r=>r.json())
  ]);
  const cfg={version:1,exported:new Date().toISOString(),
   exercise:{enabled:ex.enabled,day:ex.day,hour:ex.hour,minute:ex.minute,duration:ex.duration,load_transfer:ex.load_transfer},
   thermostat:(th.relays||[]).map(r=>({idx:r.idx,name:r.name,enabled:r.enabled,sensor:r.sensor,on_below:r.on_below,off_above:r.off_above})),
   maintenance:{items:mt.items||[],total_hours:mt.total_hours||0},
   fuel:{tank_size:fu.tank_size||0,rate_idle:fu.rate_idle||0,rate_full:fu.rate_full||0}
  };
  const blob=new Blob([JSON.stringify(cfg,null,2)],{type:'application/json'});
  const a=document.createElement('a');a.href=URL.createObjectURL(blob);a.download='genset_config_'+new Date().toISOString().slice(0,10)+'.json';a.click();URL.revokeObjectURL(a.href);
  toast('Config exported','ok');
 }catch(e){toast('Export failed','err');}
}
async function importAllConfig(ev){const file=ev.target.files[0];if(!file)return;
 const reader=new FileReader();reader.onload=async function(e){try{
  const cfg=JSON.parse(e.target.result);
  if(!cfg.version){toast('Invalid config file','err');return;}
  let ok=0,fail=0;
  // Exercise
  if(cfg.exercise){try{const r=await fetch('/api/exercise',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(cfg.exercise)});const d=await r.json();if(d.ok)ok++;else fail++;}catch(x){fail++;}}
  // Thermostat
  if(cfg.thermostat&&cfg.thermostat.length){try{const r=await fetch('/api/thermostat',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(cfg.thermostat)});const d=await r.json();if(d.ok)ok++;else fail++;}catch(x){fail++;}}
  // Maintenance
  if(cfg.maintenance&&cfg.maintenance.items){try{const r=await fetch('/api/maintenance',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'import',items:cfg.maintenance.items})});const d=await r.json();if(d.ok)ok++;else fail++;}catch(x){fail++;}}
  toast(ok+' config(s) imported'+(fail?' ('+fail+' failed)':''),fail?'err':'ok');
  loadExercise();loadThermostat();loadMaint();loadFuel();
 }catch(x){toast('Invalid JSON file','err');}};reader.readAsText(file);ev.target.value='';}

/* ── Fuel Level ── */
const FUEL_NAMES={diesel:'Diesel',propane:'Propane',natural_gas:'Natural Gas',gasoline:'Gasoline'};
function loadFuel(){fetch('/api/fuel').then(r=>r.json()).then(d=>{
 const s=document.getElementById('fuelSection');
 if(!d.tank_size&&!d.sensor){if(s)s.style.display='none';return;}
 if(s)s.style.display='';renderFuel(d);
}).catch(()=>{});}
function renderFuel(d){
 const isSensor=!!d.sensor;
 const fname=FUEL_NAMES[d.fuel_type]||d.fuel_type||'Fuel';
 const vu=d.vol_unit||'L';
 const ru=d.rate_unit||'lph';
 const title=document.getElementById('fuelTitle');
 if(title)title.textContent=fname+(isSensor?' Level':' Estimate');
 const pct=Math.max(0,Math.min(100,d.pct||0));
 const bar=document.getElementById('fuelBar');
 if(bar){bar.style.width=pct+'%';bar.style.background=pct>25?'var(--green)':pct>10?'var(--orange)':'var(--red)';}
 const t=document.getElementById('fuelPct');if(t)t.textContent=d.waiting?'Waiting...':Math.round(pct)+'%';
 const rs=document.getElementById('fuelRemainStat');if(rs)rs.style.display=(d.tank_size>0)?'':'none';
 const r=document.getElementById('fuelRemain');if(r)r.textContent=(d.remain||0).toFixed(1);
 const rvu=document.getElementById('fuelVolUnit');if(rvu)rvu.textContent=vu;
 const hasRate=(d.eff_rate>0||d.rate_idle>0||d.rate_full>0);
 const rateStat=document.getElementById('fuelRateStat');
 if(rateStat){rateStat.style.display=hasRate?'':'none';}
 const rateEl=document.getElementById('fuelRate');
 if(rateEl&&hasRate)rateEl.textContent=(d.eff_rate||0).toFixed(1);
 const rateU=document.getElementById('fuelRateUnit');if(rateU)rateU.textContent=ru;
 const rts=document.getElementById('fuelRuntimeStat');if(rts)rts.style.display=(hasRate&&d.remain>0)?'':'none';
 const rt=document.getElementById('fuelRuntime');if(rt)rt.textContent=(d.est_hours||0).toFixed(1);
 const tks=document.getElementById('fuelTankStat');if(tks)tks.style.display=(d.tank_size>0)?'':'none';
 const tk=document.getElementById('fuelTank');if(tk)tk.textContent=(d.tank_size||0).toFixed(0);
 const tku=document.getElementById('fuelTankUnit');if(tku)tku.textContent=vu;
 const fb=document.getElementById('fuelFillBtn');if(fb)fb.style.display=isSensor?'none':'';
}
function fillTank(){fetch('/api/fuel',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'fill'})})
 .then(r=>r.json()).then(d=>{toast(d.ok?'Tank marked filled':'Failed',d.ok?'ok':'err');loadFuel();}).catch(()=>toast('Error','err'));}

/* ── Runtime History Chart ── */
function loadRuntimeHistory(){fetch('/api/runtime_history').then(r=>r.json()).then(d=>{
 if(d.days&&d.days.length)renderRuntimeChart(d.days);}).catch(()=>{});}
function renderRuntimeChart(days){
 const svg=document.getElementById('runtimeChart');if(!svg||!days.length)return;
 const W=svg.clientWidth||600,H=140,pt=20,pb=20,pl=35,pr=10;
 const cW=W-pl-pr,cH=H-pt-pb,maxH=Math.max(...days.map(d=>d.h),0.1);
 const bw=Math.max(6,Math.floor(cW/days.length*0.65)),step=Math.floor(cW/days.length);
 let h='';for(let i=0;i<=4;i++){const y=pt+cH*(1-i/4);const v=(maxH*i/4).toFixed(maxH>=10?0:1);
  h+='<line x1="'+pl+'" y1="'+y+'" x2="'+(W-pr)+'" y2="'+y+'" stroke="#252a3a"/>';
  h+='<text x="'+(pl-4)+'" y="'+(y+3)+'" fill="#6b7394" font-size="9" text-anchor="end">'+v+'</text>';}
 days.forEach((d,i)=>{const x=pl+i*step+(step-bw)/2;const bh=d.h/maxH*cH;const y=pt+cH-bh;
  h+='<rect x="'+x+'" y="'+y+'" width="'+bw+'" height="'+Math.max(bh,1)+'" fill="var(--blue)" rx="2" opacity=".8"><title>'+d.d+': '+d.h.toFixed(1)+'h</title></rect>';
  if(days.length<=14||i%Math.ceil(days.length/7)===0)h+='<text x="'+(x+bw/2)+'" y="'+(H-4)+'" fill="#6b7394" font-size="8" text-anchor="middle">'+d.d.slice(5)+'</text>';});
 svg.innerHTML=h;}

/* ── Multi-language ── */
const I18N={es:{'Monitoring':'Monitoreo','Configuration':'Configuraci\u00f3n','History':'Historial','Controls':'Controles','Generator Output':'Salida Generador','Engine':'Motor','Alarms':'Alarmas','Inputs / Outputs':'E/S','Fuel Estimate':'Combustible','Exercise Schedule':'Ejercicio','Relay Thermostat':'Termostato','Maintenance Tracker':'Mantenimiento','Runtime &amp; Totals':'Totales','Runtime History':'Historial Ejecuci\u00f3n','Event Log':'Registro','Board Relays':'Rel\u00e9s'},fr:{'Monitoring':'Surveillance','Configuration':'Configuration','History':'Historique','Controls':'Commandes','Generator Output':'Sortie G\u00e9n\u00e9rateur','Engine':'Moteur','Alarms':'Alarmes','Inputs / Outputs':'E/S','Fuel Estimate':'Carburant','Exercise Schedule':'Exercice','Relay Thermostat':'Thermostat','Maintenance Tracker':'Maintenance','Runtime &amp; Totals':'Totaux','Runtime History':'Historique','Event Log':'Journal','Board Relays':'Relais'}};
let i18nDone=false;
function applyI18n(lang){if(i18nDone||!lang||lang==='en')return;const t=I18N[lang];if(!t)return;i18nDone=true;
 document.querySelectorAll('.nav-tab').forEach(e=>{const k=e.textContent.trim();if(t[k])e.textContent=t[k];});
 document.querySelectorAll('.card-hd h2').forEach(e=>{const k=e.innerHTML.trim();if(t[k])e.innerHTML=t[k];});}
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
  this->init_maint_defaults_();
  this->load_maintenance_config_();
  this->load_fuel_config_();
  this->load_runtime_history_();
  this->init_buzzer_();
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
  // Check runtime history every 60 seconds
  if (now - this->last_hist_check_ >= 60000) {
    this->last_hist_check_ = now;
    this->check_runtime_history_();
  }
  // Sample battery voltage every 30 seconds
  if (now - this->last_batt_sample_ >= 30000) {
    this->last_batt_sample_ = now;
    this->sample_battery_voltage_();
  }
}

float SmartgenHSC941Web::get_effective_burn_rate() const {
  float idle = this->burn_rate_idle_;
  float full = this->burn_rate_full_;
  if (idle <= 0 && full <= 0)
    return 0;
  float load_pct = 0;
  if (this->controller_) {
    auto *s = this->controller_->get_output_load_percent_sensor();
    if (s && s->has_state() && !std::isnan(s->state)) {
      load_pct = s->state;
      if (load_pct < 0) load_pct = 0;
      if (load_pct > 100) load_pct = 100;
    }
  }
  return idle + (full - idle) * (load_pct / 100.0f);
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
  if (this->tank_size_liters_ > 0) {
    const char *u = (this->fuel_unit_ == "gph") ? "gal" : "L";
    const char *ru = (this->fuel_unit_ == "gph") ? "gph" : "lph";
    ESP_LOGCONFIG(TAG, "  Fuel: %.0f%s tank, %s, idle %.2f %s, full %.2f %s",
                  this->tank_size_liters_, u, this->fuel_type_.c_str(),
                  this->burn_rate_idle_, ru, this->burn_rate_full_, ru);
  }
  ESP_LOGCONFIG(TAG, "  Language: %s", this->language_.c_str());
  if (this->mains_sensor_) {
    ESP_LOGCONFIG(TAG, "  Mains sensor: configured");
  }
  if (this->buzzer_pin_ >= 0) {
    ESP_LOGCONFIG(TAG, "  Buzzer: GPIO%d", this->buzzer_pin_);
  }
  if (!this->pin_code_.empty()) {
    ESP_LOGCONFIG(TAG, "  PIN lock: configured (4-digit)");
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

  // Board buzzer alarm
  bool any_alarm = shutdown || estop;
  if (any_alarm && !this->buzzer_silenced_ && this->buzzer_pin_ >= 0 && !this->buzzer_active_) {
    this->buzzer_start_();
    this->buzzer_active_ = true;
    ESP_LOGW(TAG, "Alarm buzzer activated");
  }
  if (!any_alarm && this->buzzer_active_) {
    this->buzzer_stop_();
    this->buzzer_active_ = false;
    this->buzzer_silenced_ = false;
    ESP_LOGI(TAG, "Alarm buzzer deactivated");
  }
}

// ============================================================
//  Buzzer — direct LEDC tone on dedicated timer/channel
// ============================================================
static const ledc_timer_t BUZZER_TIMER = LEDC_TIMER_3;
static const ledc_channel_t BUZZER_CHANNEL = LEDC_CHANNEL_7;
static const ledc_mode_t BUZZER_SPEED = LEDC_LOW_SPEED_MODE;

void SmartgenHSC941Web::init_buzzer_() {
  if (this->buzzer_pin_ < 0) return;

  ledc_timer_config_t timer_conf = {};
  timer_conf.speed_mode = BUZZER_SPEED;
  timer_conf.timer_num = BUZZER_TIMER;
  timer_conf.duty_resolution = LEDC_TIMER_10_BIT;
  timer_conf.freq_hz = 2700;  // Typical piezo resonance frequency
  timer_conf.clk_cfg = LEDC_AUTO_CLK;
  esp_err_t err = ledc_timer_config(&timer_conf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Buzzer LEDC timer config failed: %s", esp_err_to_name(err));
    this->buzzer_pin_ = -1;
    return;
  }

  ledc_channel_config_t chan_conf = {};
  chan_conf.speed_mode = BUZZER_SPEED;
  chan_conf.channel = BUZZER_CHANNEL;
  chan_conf.timer_sel = BUZZER_TIMER;
  chan_conf.intr_type = LEDC_INTR_DISABLE;
  chan_conf.gpio_num = this->buzzer_pin_;
  chan_conf.duty = 0;
  chan_conf.hpoint = 0;
  err = ledc_channel_config(&chan_conf);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Buzzer LEDC channel config failed: %s", esp_err_to_name(err));
    this->buzzer_pin_ = -1;
    return;
  }

  ESP_LOGI(TAG, "Buzzer initialized on GPIO%d (2700 Hz, LEDC timer %d channel %d)",
           this->buzzer_pin_, BUZZER_TIMER, BUZZER_CHANNEL);
}

void SmartgenHSC941Web::buzzer_start_() {
  if (this->buzzer_pin_ < 0) return;
  ledc_set_duty(BUZZER_SPEED, BUZZER_CHANNEL, 512);  // 50% of 1024 (10-bit)
  ledc_update_duty(BUZZER_SPEED, BUZZER_CHANNEL);
}

void SmartgenHSC941Web::buzzer_stop_() {
  if (this->buzzer_pin_ < 0) return;
  ledc_set_duty(BUZZER_SPEED, BUZZER_CHANNEL, 0);
  ledc_update_duty(BUZZER_SPEED, BUZZER_CHANNEL);
}

// ============================================================
//  Maintenance Tracker — NVS-backed service intervals
// ============================================================
static const char *NVS_MAINT_NS = "genset_mt";
static const char *NVS_MAINT_KEY = "config";

void SmartgenHSC941Web::init_maint_defaults_() {
  this->maint_items_[0] = {"Oil Change", 250, 0};
  this->maint_items_[1] = {"Air Filter", 500, 0};
  this->maint_items_[2] = {"Coolant Service", 1000, 0};
  this->maint_items_[3] = {"Belt Inspection", 500, 0};
  this->maint_items_[4] = {"Battery Service", 500, 0};
}

void SmartgenHSC941Web::load_maintenance_config_() {
  nvs_handle_t handle;
  if (nvs_open(NVS_MAINT_NS, NVS_READONLY, &handle) != ESP_OK) return;
  size_t len = 0;
  if (nvs_get_str(handle, NVS_MAINT_KEY, nullptr, &len) == ESP_OK && len > 0) {
    char *buf = new char[len];
    nvs_get_str(handle, NVS_MAINT_KEY, buf, &len);
    // Parse: [{"n":"Oil Change","i":250,"r":0.0}, ...]
    const char *p = buf;
    uint8_t idx = 0;
    while (idx < MAX_MAINT_ITEMS) {
      const char *start = strchr(p, '{');
      if (!start) break;
      const char *end = strchr(start, '}');
      if (!end) break;
      auto extract_str = [&](const char *key) -> std::string {
        const char *k = strstr(start, key);
        if (!k || k >= end) return "";
        k += strlen(key);
        const char *q = strchr(k, '"');
        return (q && q < end) ? std::string(k, q) : "";
      };
      auto extract_num = [&](const char *key) -> float {
        const char *k = strstr(start, key);
        if (!k || k >= end) return 0;
        return strtof(k + strlen(key), nullptr);
      };
      std::string name = extract_str("\"n\":\"");
      if (!name.empty()) this->maint_items_[idx].name = name;
      float iv = extract_num("\"i\":");
      if (iv > 0) this->maint_items_[idx].interval_hours = (uint16_t)iv;
      this->maint_items_[idx].last_reset_hours = extract_num("\"r\":");
      idx++;
      p = end + 1;
    }
    delete[] buf;
    ESP_LOGI(TAG, "Loaded maintenance config for %u items", idx);
  }
  nvs_close(handle);
}

void SmartgenHSC941Web::save_maintenance_config_() {
  nvs_handle_t handle;
  if (nvs_open(NVS_MAINT_NS, NVS_READWRITE, &handle) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to open NVS for maintenance write");
    return;
  }
  std::string json;
  json.reserve(512);
  json += '[';
  for (uint8_t i = 0; i < MAX_MAINT_ITEMS; i++) {
    if (i > 0) json += ',';
    const auto &m = this->maint_items_[i];
    json += "{\"n\":\"";
    json += m.name;
    json += "\",\"i\":";
    json += std::to_string(m.interval_hours);
    json += ",\"r\":";
    char fb[16];
    snprintf(fb, sizeof(fb), "%.1f", m.last_reset_hours);
    json += fb;
    json += '}';
  }
  json += ']';
  nvs_set_str(handle, NVS_MAINT_KEY, json.c_str());
  nvs_commit(handle);
  nvs_close(handle);
  ESP_LOGI(TAG, "Saved maintenance config");
}

// ============================================================
//  Fuel Estimator — NVS-backed
// ============================================================
static const char *NVS_FUEL_NS = "genset_fuel";
static const char *NVS_FUEL_KEY = "fill_hrs";

void SmartgenHSC941Web::load_fuel_config_() {
  if (this->tank_size_liters_ <= 0) return;  // Not configured
  nvs_handle_t handle;
  if (nvs_open(NVS_FUEL_NS, NVS_READONLY, &handle) != ESP_OK) return;
  size_t len = 0;
  if (nvs_get_str(handle, NVS_FUEL_KEY, nullptr, &len) == ESP_OK && len > 0) {
    char buf[32];
    nvs_get_str(handle, NVS_FUEL_KEY, buf, &len);
    this->last_fill_hours_ = strtof(buf, nullptr);
    ESP_LOGI(TAG, "Fuel: last fill at %.1f hours", this->last_fill_hours_);
  }
  nvs_close(handle);
}

void SmartgenHSC941Web::save_fuel_config_() {
  nvs_handle_t handle;
  if (nvs_open(NVS_FUEL_NS, NVS_READWRITE, &handle) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to open NVS for fuel write");
    return;
  }
  char buf[32];
  snprintf(buf, sizeof(buf), "%.1f", this->last_fill_hours_);
  nvs_set_str(handle, NVS_FUEL_KEY, buf);
  nvs_commit(handle);
  nvs_close(handle);
}

// ============================================================
//  Runtime History — SPIFFS-backed daily totals
// ============================================================
static const char *RUNTIME_HIST_PATH = "/eventlog/runtime.dat";
static const uint8_t MAX_RUNTIME_DAYS = 30;

void SmartgenHSC941Web::load_runtime_history_() {
  if (!this->spiffs_mounted_) return;
  FILE *f = fopen(RUNTIME_HIST_PATH, "r");
  if (!f) return;
  char line[32];
  while (fgets(line, sizeof(line), f)) {
    char *tab = strchr(line, '\t');
    if (!tab) continue;
    *tab = '\0';
    RuntimeDay rd;
    strncpy(rd.date, line, sizeof(rd.date) - 1);
    rd.date[sizeof(rd.date) - 1] = '\0';
    rd.hours = strtof(tab + 1, nullptr);
    this->runtime_history_.push_back(rd);
  }
  fclose(f);
  // Trim to max
  while (this->runtime_history_.size() > MAX_RUNTIME_DAYS) {
    this->runtime_history_.erase(this->runtime_history_.begin());
  }
  ESP_LOGI(TAG, "Loaded %u runtime history entries", (unsigned)this->runtime_history_.size());
}

void SmartgenHSC941Web::save_runtime_day_(const char *date, float hours) {
  if (!this->spiffs_mounted_) return;
  // Check if last entry is same date — update it
  if (!this->runtime_history_.empty()) {
    auto &last = this->runtime_history_.back();
    if (strcmp(last.date, date) == 0) {
      last.hours = hours;
      // Rewrite entire file
      FILE *f = fopen(RUNTIME_HIST_PATH, "w");
      if (f) {
        for (const auto &rd : this->runtime_history_) {
          fprintf(f, "%s\t%.2f\n", rd.date, rd.hours);
        }
        fclose(f);
      }
      return;
    }
  }
  // New day
  RuntimeDay rd;
  strncpy(rd.date, date, sizeof(rd.date) - 1);
  rd.date[sizeof(rd.date) - 1] = '\0';
  rd.hours = hours;
  this->runtime_history_.push_back(rd);
  while (this->runtime_history_.size() > MAX_RUNTIME_DAYS) {
    this->runtime_history_.erase(this->runtime_history_.begin());
  }
  // Rewrite file
  FILE *f = fopen(RUNTIME_HIST_PATH, "w");
  if (f) {
    for (const auto &entry : this->runtime_history_) {
      fprintf(f, "%s\t%.2f\n", entry.date, entry.hours);
    }
    fclose(f);
  }
}

void SmartgenHSC941Web::check_runtime_history_() {
  if (!this->controller_ || !this->controller_->is_connected()) return;
  auto *hrs_sensor = this->controller_->get_total_hours_sensor();
  if (!hrs_sensor || !hrs_sensor->has_state()) return;
  float total_hrs = hrs_sensor->state;

  time_t now_t = time(nullptr);
  if (now_t < 1704067200) return;  // Time not synced
  struct tm *tm = localtime(&now_t);
  if (!tm) return;

  uint8_t today = tm->tm_mday;
  char date_str[11];
  strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm);

  if (this->prev_hist_day_ == 255) {
    // First check after boot — just record baseline
    this->prev_total_hours_ = total_hrs;
    this->prev_hist_day_ = today;
    return;
  }

  // Compute today's runtime delta
  float delta = total_hrs - this->prev_total_hours_;
  if (delta < 0) delta = 0;

  // Save/update today's entry
  if (delta > 0.01f || today != this->prev_hist_day_) {
    this->save_runtime_day_(date_str, delta);
  }

  if (today != this->prev_hist_day_) {
    // New day — reset baseline
    this->prev_total_hours_ = total_hrs;
    this->prev_hist_day_ = today;
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
  config.max_uri_handlers = 20;
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

  // Manifest (PWA)
  httpd_uri_t manifest_uri = {
      .uri = "/manifest.json",
      .method = HTTP_GET,
      .handler = SmartgenHSC941Web::handle_manifest_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &manifest_uri);

  // Maintenance
  httpd_uri_t maint_get_uri = {
      .uri = "/api/maintenance",
      .method = HTTP_GET,
      .handler = SmartgenHSC941Web::handle_api_maintenance_get_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &maint_get_uri);

  httpd_uri_t maint_post_uri = {
      .uri = "/api/maintenance",
      .method = HTTP_POST,
      .handler = SmartgenHSC941Web::handle_api_maintenance_post_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &maint_post_uri);

  // Fuel
  httpd_uri_t fuel_get_uri = {
      .uri = "/api/fuel",
      .method = HTTP_GET,
      .handler = SmartgenHSC941Web::handle_api_fuel_get_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &fuel_get_uri);

  httpd_uri_t fuel_post_uri = {
      .uri = "/api/fuel",
      .method = HTTP_POST,
      .handler = SmartgenHSC941Web::handle_api_fuel_post_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &fuel_post_uri);

  // Runtime History
  httpd_uri_t runtime_hist_uri = {
      .uri = "/api/runtime_history",
      .method = HTTP_GET,
      .handler = SmartgenHSC941Web::handle_api_runtime_history_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &runtime_hist_uri);

  // Battery voltage history
  httpd_uri_t batt_hist_uri = {
      .uri = "/api/battery_history",
      .method = HTTP_GET,
      .handler = SmartgenHSC941Web::handle_api_battery_history_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &batt_hist_uri);

  // Buzzer
  httpd_uri_t buzzer_post_uri = {
      .uri = "/api/buzzer",
      .method = HTTP_POST,
      .handler = SmartgenHSC941Web::handle_api_buzzer_post_,
      .user_ctx = this,
  };
  httpd_register_uri_handler(this->server_, &buzzer_post_uri);
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
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  // Inject YAML-configured PIN code before </head> if set
  if (self && !self->pin_code_.empty()) {
    const char *head_end = strstr(DASHBOARD_HTML, "</head>");
    if (head_end) {
      httpd_resp_send_chunk(req, DASHBOARD_HTML, head_end - DASHBOARD_HTML);
      char inject[64];
      snprintf(inject, sizeof(inject), "<script>var cfgPin='%s';</script>", self->pin_code_.c_str());
      httpd_resp_send_chunk(req, inject, strlen(inject));
      httpd_resp_send_chunk(req, head_end, strlen(head_end));
      return httpd_resp_send_chunk(req, nullptr, 0);
    }
  }
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
    json += ",\"ambient_temp_name\":\"";
    json += self->ambient_temp_name_;
    json += "\"";
    json += '}';
  }

  // Append ambient humidity if configured
  if (self->ambient_humidity_ != nullptr) {
    if (!json.empty() && json.back() == '}') json.pop_back();
    json += ",\"ambient_humidity\":";
    if (self->ambient_humidity_->has_state()) {
      char buf[16];
      snprintf(buf, sizeof(buf), "%.1f", self->ambient_humidity_->state);
      json += buf;
    } else {
      json += "null";
    }
    json += ",\"ambient_humidity_name\":\"";
    json += self->ambient_humidity_name_;
    json += "\"";
    json += '}';
  }

  // Append single_phase flag
  if (self->single_phase_) {
    if (!json.empty() && json.back() == '}') json.pop_back();
    json += ",\"single_phase\":true}";
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

  // Append language
  if (self->language_ != "en") {
    if (!json.empty() && json.back() == '}') json.pop_back();
    json += ",\"language\":\"";
    json += self->language_;
    json += "\"}";
  }

  // Append mains sensor state
  if (self->mains_sensor_ != nullptr) {
    if (!json.empty() && json.back() == '}') json.pop_back();
    json += ",\"mains_ok\":";
    json += self->mains_sensor_->state ? "true" : "false";
    json += '}';
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
    json += ",{\"key\":\"ambient_temp\",\"label\":\"";
    json += self->get_ambient_temp_name();
    json += "\"}";
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

// ============================================================
//  PWA Manifest handler
// ============================================================

esp_err_t SmartgenHSC941Web::handle_manifest_(httpd_req_t *req) {
  static const char MANIFEST[] = R"({"name":"HSC941 Genset Controller","short_name":"Genset","display":"standalone","background_color":"#0c0e14","theme_color":"#0c0e14","start_url":"/","icons":[]})";
  httpd_resp_set_type(req, "application/json");
  return httpd_resp_send(req, MANIFEST, strlen(MANIFEST));
}

// ============================================================
//  Maintenance API handlers
// ============================================================

esp_err_t SmartgenHSC941Web::handle_api_maintenance_get_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

  float total_hrs = 0;
  if (self->controller_) {
    auto *s = self->controller_->get_total_hours_sensor();
    if (s && s->has_state()) total_hrs = s->state;
  }

  std::string json;
  json.reserve(512);
  char fb[16];
  snprintf(fb, sizeof(fb), "%.1f", total_hrs);
  json += "{\"total_hours\":";
  json += fb;
  json += ",\"items\":[";
  const auto &items = self->get_maint_items();
  for (uint8_t i = 0; i < MAX_MAINT_ITEMS; i++) {
    if (i > 0) json += ',';
    json += "{\"name\":\"";
    json += items[i].name;
    json += "\",\"interval\":";
    json += std::to_string(items[i].interval_hours);
    json += ",\"reset\":";
    snprintf(fb, sizeof(fb), "%.1f", items[i].last_reset_hours);
    json += fb;
    json += '}';
  }
  json += "]}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json.c_str(), json.size());
}

esp_err_t SmartgenHSC941Web::handle_api_maintenance_post_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

  int content_len = req->content_len;
  if (content_len <= 0 || content_len > 1024) {
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

  // Parse {"action":"reset","index":N}
  if (strstr(body, "\"reset\"")) {
    const char *idx_p = strstr(body, "\"index\":");
    if (idx_p) {
      int idx = atoi(idx_p + 8);
      if (idx >= 0 && idx < MAX_MAINT_ITEMS) {
        float total_hrs = 0;
        if (self->controller_) {
          auto *s = self->controller_->get_total_hours_sensor();
          if (s && s->has_state()) total_hrs = s->state;
        }
        self->maint_items_[idx].last_reset_hours = total_hrs;
        self->save_maintenance_config_();
        char msg[64];
        snprintf(msg, sizeof(msg), "Maintenance reset: %s at %.0fh",
                 self->maint_items_[idx].name.c_str(), total_hrs);
        self->log_event(msg);
      }
    }
  }
  // Parse {"action":"update","index":N,"interval":V}
  else if (strstr(body, "\"update\"")) {
    const char *idx_p = strstr(body, "\"index\":");
    const char *iv_p = strstr(body, "\"interval\":");
    if (idx_p && iv_p) {
      int idx = atoi(idx_p + 8);
      int iv = atoi(iv_p + 11);
      if (idx >= 0 && idx < MAX_MAINT_ITEMS && iv > 0 && iv <= 9999) {
        self->maint_items_[idx].interval_hours = (uint16_t) iv;
        self->save_maintenance_config_();
        ESP_LOGI(TAG, "Maintenance interval updated: %s = %dh",
                 self->maint_items_[idx].name.c_str(), iv);
      }
    }
  }
  // Parse {"action":"import","items":[{"name":...,"interval":...,"reset":...}]}
  else if (strstr(body, "\"import\"")) {
    const char *p = strstr(body, "\"items\"");
    if (p) {
      uint8_t idx = 0;
      const char *cur = p;
      while (idx < MAX_MAINT_ITEMS) {
        const char *start = strchr(cur, '{');
        if (!start) break;
        const char *end = strchr(start, '}');
        if (!end) break;
        // Extract "name":"..."
        const char *nk = strstr(start, "\"name\":\"");
        if (nk && nk < end) {
          nk += 8;
          const char *nq = strchr(nk, '"');
          if (nq && nq < end) {
            self->maint_items_[idx].name = std::string(nk, nq);
          }
        }
        // Extract "interval":N
        const char *ik = strstr(start, "\"interval\":");
        if (ik && ik < end) {
          int iv = atoi(ik + 11);
          if (iv > 0 && iv <= 9999) self->maint_items_[idx].interval_hours = (uint16_t) iv;
        }
        // Extract "reset":N
        const char *rk = strstr(start, "\"reset\":");
        if (rk && rk < end) {
          self->maint_items_[idx].last_reset_hours = strtof(rk + 8, nullptr);
        }
        idx++;
        cur = end + 1;
      }
      self->save_maintenance_config_();
      ESP_LOGI(TAG, "Maintenance config imported (%u items)", idx);
    }
  }

  delete[] body;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  const char *r = R"({"ok":true})";
  return httpd_resp_send(req, r, strlen(r));
}

// ============================================================
//  Fuel API handlers
// ============================================================

esp_err_t SmartgenHSC941Web::handle_api_fuel_get_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

  const auto &ftype = self->get_fuel_type();
  const auto &funit = self->get_fuel_unit();
  const char *vol_unit = (funit == "gph") ? "gal" : "L";
  const char *rate_unit = (funit == "gph") ? "gph" : "lph";

  // Get current load percentage for burn rate interpolation
  float load_pct = 0;
  bool engine_on = false;
  if (self->controller_) {
    auto *load_s = self->controller_->get_output_load_percent_sensor();
    if (load_s && load_s->has_state() && !std::isnan(load_s->state)) {
      load_pct = load_s->state;
      if (load_pct < 0) load_pct = 0;
      if (load_pct > 100) load_pct = 100;
    }
    // Check if engine is running (engine_running_status > 0 means running)
    auto *eng_s = self->controller_->get_engine_running_status_sensor();
    if (eng_s && eng_s->has_state() && eng_s->state > 0) engine_on = true;
  }

  float rate_idle = self->get_burn_rate_idle();
  float rate_full = self->get_burn_rate_full();
  bool has_rates = (rate_idle > 0 || rate_full > 0);
  // Effective burn rate: linear interpolation idle→full by load%
  float eff_rate = 0;
  if (has_rates && engine_on) {
    eff_rate = rate_idle + (rate_full - rate_idle) * (load_pct / 100.0f);
  } else if (has_rates) {
    eff_rate = rate_idle;  // Engine off — show idle rate
  }

  // Mode 1: Real fuel level sensor (e.g. Mopeka propane sensor → percentage)
  auto *fuel_sens = self->get_fuel_level_sensor();
  if (fuel_sens && fuel_sens->has_state() && !std::isnan(fuel_sens->state)) {
    float pct = fuel_sens->state;
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    float tank = self->get_tank_size();
    float remain = (tank > 0) ? tank * pct / 100.0f : 0;
    float est_hours = (eff_rate > 0 && remain > 0) ? remain / eff_rate : 0;

    char json[512];
    snprintf(json, sizeof(json),
      "{\"sensor\":true,\"fuel_type\":\"%s\",\"vol_unit\":\"%s\",\"rate_unit\":\"%s\","
      "\"tank_size\":%.1f,\"rate_idle\":%.2f,\"rate_full\":%.2f,\"eff_rate\":%.2f,\"load\":%.0f,"
      "\"pct\":%.1f,\"remain\":%.1f,\"est_hours\":%.1f}",
      ftype.c_str(), vol_unit, rate_unit, tank, rate_idle, rate_full, eff_rate, load_pct,
      pct, remain, est_hours);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json, strlen(json));
  }

  // Mode 2: Burn-rate estimation (fallback)
  float tank = self->get_tank_size();
  if (tank <= 0 || !has_rates) {
    // Check if we have a fuel sensor configured but it hasn't reported yet
    if (fuel_sens) {
      char wjson[256];
      snprintf(wjson, sizeof(wjson),
        "{\"sensor\":true,\"fuel_type\":\"%s\",\"vol_unit\":\"%s\",\"rate_unit\":\"%s\",\"tank_size\":0,\"pct\":0,\"waiting\":true}",
        ftype.c_str(), vol_unit, rate_unit);
      httpd_resp_set_type(req, "application/json");
      return httpd_resp_send(req, wjson, strlen(wjson));
    }
    const char *r = R"({"tank_size":0})";
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, r, strlen(r));
  }

  // For estimation mode, use average of idle+full as a rough rate for consumed calc
  float avg_rate = (rate_idle + rate_full) / 2.0f;
  if (avg_rate <= 0) avg_rate = rate_idle > 0 ? rate_idle : rate_full;

  float total_hrs = 0;
  if (self->controller_) {
    auto *s = self->controller_->get_total_hours_sensor();
    if (s && s->has_state()) total_hrs = s->state;
  }

  float consumed = (total_hrs - self->get_last_fill_hours()) * avg_rate;
  if (consumed < 0) consumed = 0;
  float remain = tank - consumed;
  if (remain < 0) remain = 0;
  float pct = (remain / tank) * 100.0f;
  float est_hours = (eff_rate > 0) ? remain / eff_rate : (avg_rate > 0 ? remain / avg_rate : 0);

  char json[512];
  snprintf(json, sizeof(json),
    "{\"fuel_type\":\"%s\",\"vol_unit\":\"%s\",\"rate_unit\":\"%s\","
    "\"tank_size\":%.1f,\"rate_idle\":%.2f,\"rate_full\":%.2f,\"eff_rate\":%.2f,\"load\":%.0f,"
    "\"pct\":%.1f,\"remain\":%.1f,\"est_hours\":%.1f,\"consumed\":%.1f}",
    ftype.c_str(), vol_unit, rate_unit, tank, rate_idle, rate_full, eff_rate, load_pct,
    pct, remain, est_hours, consumed);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json, strlen(json));
}

esp_err_t SmartgenHSC941Web::handle_api_fuel_post_(httpd_req_t *req) {
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

  if (strstr(body, "\"fill\"")) {
    float total_hrs = 0;
    if (self->controller_) {
      auto *s = self->controller_->get_total_hours_sensor();
      if (s && s->has_state()) total_hrs = s->state;
    }
    self->last_fill_hours_ = total_hrs;
    self->save_fuel_config_();
    self->log_event("Fuel tank marked as filled");
  }

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  const char *r = R"({"ok":true})";
  return httpd_resp_send(req, r, strlen(r));
}

// ============================================================
//  Battery voltage sampling & API
// ============================================================

void SmartgenHSC941Web::sample_battery_voltage_() {
  if (!this->controller_) return;
  auto *sens = this->controller_->get_battery_voltage_sensor();
  if (!sens || !sens->has_state()) return;
  float v = sens->state;
  if (v <= 0 || std::isnan(v)) return;

  // Push into circular buffer
  this->batt_hist_buf_[this->batt_hist_head_] = v;
  this->batt_hist_head_ = (this->batt_hist_head_ + 1) % BATT_HIST_SIZE;
  if (this->batt_hist_count_ < BATT_HIST_SIZE)
    this->batt_hist_count_++;

  // Update 24h running stats
  if (v < this->batt_24h_min_) this->batt_24h_min_ = v;
  if (v > this->batt_24h_max_) this->batt_24h_max_ = v;
  this->batt_24h_sum_ += v;
  this->batt_24h_count_++;
}

esp_err_t SmartgenHSC941Web::handle_api_battery_history_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

  std::string json;
  json.reserve(2048);
  json += "{\"samples\":[";

  size_t n = self->batt_hist_count_;
  if (n > 0) {
    // Read oldest to newest from the circular buffer
    size_t start = (self->batt_hist_head_ + BATT_HIST_SIZE - n) % BATT_HIST_SIZE;
    for (size_t i = 0; i < n; i++) {
      if (i > 0) json += ',';
      char fb[16];
      snprintf(fb, sizeof(fb), "%.2f", self->batt_hist_buf_[(start + i) % BATT_HIST_SIZE]);
      json += fb;
    }
  }

  json += "],\"min\":";
  char fb[16];
  if (self->batt_24h_count_ > 0) {
    snprintf(fb, sizeof(fb), "%.2f", self->batt_24h_min_);
    json += fb;
    json += ",\"max\":";
    snprintf(fb, sizeof(fb), "%.2f", self->batt_24h_max_);
    json += fb;
    json += ",\"avg\":";
    snprintf(fb, sizeof(fb), "%.2f", (float)(self->batt_24h_sum_ / self->batt_24h_count_));
    json += fb;
  } else {
    json += "0,\"max\":0,\"avg\":0";
  }
  json += ",\"count\":";
  char cb[12];
  snprintf(cb, sizeof(cb), "%u", (unsigned)self->batt_24h_count_);
  json += cb;
  json += '}';

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json.c_str(), json.size());
}

// ============================================================
//  Runtime History API handler
// ============================================================

esp_err_t SmartgenHSC941Web::handle_api_runtime_history_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }

  const auto &hist = self->get_runtime_history();
  std::string json;
  json.reserve(512);
  json += "{\"days\":[";
  for (size_t i = 0; i < hist.size(); i++) {
    if (i > 0) json += ',';
    char fb[16];
    snprintf(fb, sizeof(fb), "%.2f", hist[i].hours);
    json += "{\"d\":\"";
    json += hist[i].date;
    json += "\",\"h\":";
    json += fb;
    json += '}';
  }
  json += "]}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json.c_str(), json.size());
}

esp_err_t SmartgenHSC941Web::handle_api_buzzer_post_(httpd_req_t *req) {
  auto *self = static_cast<SmartgenHSC941Web *>(req->user_ctx);
  if (!self) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not available");
    return ESP_FAIL;
  }
  int content_len = req->content_len;
  if (content_len <= 0 || content_len > 256) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid body");
    return ESP_FAIL;
  }
  char body[257];
  int received = httpd_req_recv(req, body, content_len);
  if (received <= 0) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Read error");
    return ESP_FAIL;
  }
  body[received] = '\0';

  bool ok = false;
  if (strstr(body, "\"silence\"")) {
    self->buzzer_stop_();
    self->buzzer_active_ = false;
    self->buzzer_silenced_ = true;
    ok = true;
    self->log_event("Alarm buzzer silenced");
  } else if (strstr(body, "\"test\"")) {
    if (self->buzzer_pin_ >= 0) {
      self->buzzer_start_();
      self->buzzer_active_ = true;
      ok = true;
    }
  } else if (strstr(body, "\"stop\"")) {
    self->buzzer_stop_();
    self->buzzer_active_ = false;
    ok = true;
  }

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  const char *resp = ok ? R"({"ok":true})" : R"({"ok":false})";
  return httpd_resp_send(req, resp, strlen(resp));
}

}  // namespace smartgen_hsc941_web
}  // namespace esphome
