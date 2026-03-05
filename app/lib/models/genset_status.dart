/// Data models matching the ESP32 /api/status JSON response.
library;

class GensetStatus {
  final bool connected;
  final Map<String, double?> sensors;
  final Map<String, bool> binary;
  final double? ambientTemp;
  final double? ambientHumidity;
  final bool? singlePhase;
  final bool? mainsOk;
  final List<RelayState> relays;

  GensetStatus({
    required this.connected,
    required this.sensors,
    required this.binary,
    this.ambientTemp,
    this.ambientHumidity,
    this.singlePhase,
    this.mainsOk,
    this.relays = const [],
  });

  factory GensetStatus.fromJson(Map<String, dynamic> json) {
    final sensorsRaw = json['sensors'] as Map<String, dynamic>? ?? {};
    final binaryRaw = json['binary'] as Map<String, dynamic>? ?? {};

    final sensors = <String, double?>{};
    for (final e in sensorsRaw.entries) {
      sensors[e.key] = (e.value is num) ? (e.value as num).toDouble() : null;
    }

    final binary = <String, bool>{};
    for (final e in binaryRaw.entries) {
      binary[e.key] = e.value == true;
    }

    final relaysList = <RelayState>[];
    if (json['relays'] is List) {
      for (final r in json['relays'] as List) {
        relaysList.add(RelayState.fromJson(r as Map<String, dynamic>));
      }
    }

    return GensetStatus(
      connected: json['connected'] == true,
      sensors: sensors,
      binary: binary,
      ambientTemp: (json['ambient_temp'] is num)
          ? (json['ambient_temp'] as num).toDouble()
          : null,
      ambientHumidity: (json['ambient_humidity'] is num)
          ? (json['ambient_humidity'] as num).toDouble()
          : null,
      singlePhase: json['single_phase'] as bool?,
      mainsOk: json['mains_ok'] as bool?,
      relays: relaysList,
    );
  }

  // Convenience getters for common sensor values
  double? get genVoltageA => sensors['gen_va'];
  double? get genVoltageB => sensors['gen_vb'];
  double? get genVoltageC => sensors['gen_vc'];
  double? get genFrequency => sensors['gen_freq'];
  double? get engineSpeed => sensors['rpm'];
  double? get outputLoad => sensors['load_pct'];
  double? get waterTemp => sensors['water_temp'];
  double? get oilPressure => sensors['oil_press'];
  double? get batteryVoltage => sensors['batt_v'];
  double? get chargeVoltage => sensors['charge_v'];
  double? get totalActivePower => sensors['total_kw'];
  double? get reactivePower => sensors['react_kvar'];
  double? get apparentPower => sensors['apparent_kva'];
  double? get powerFactor => sensors['pf'];
  double? get totalHours => sensors['total_hrs'];
  double? get totalStarts => sensors['total_starts'];
  double? get totalEnergy => sensors['total_kwh'];
  double? get phaseACurrent => sensors['i_a'];
  double? get phaseBCurrent => sensors['i_b'];
  double? get phaseCCurrent => sensors['i_c'];
  double? get phaseAPower => sensors['kw_a'];
  double? get phaseBPower => sensors['kw_b'];
  double? get phaseCPower => sensors['kw_c'];
  double? get voltageAB => sensors['v_ab'];
  double? get voltageBC => sensors['v_bc'];
  double? get voltageCA => sensors['v_ca'];

  // Binary sensor convenience
  bool get isAlarm => binary['alarm'] == true;
  bool get isWarning => binary['warning'] == true;
  bool get isShutdown => binary['shutdown'] == true;
  bool get isAutoMode => binary['auto_mode'] == true;
  bool get isManualMode => binary['manual_mode'] == true;
  bool get isStopMode => binary['stop_mode'] == true;
  bool get isGenOnLoad => binary['gen_on_load'] == true;
  bool get isEmergencyStop => binary['estop'] == true;
  bool get isEngineRunning => (engineSpeed ?? 0) > 100;

  String get modeString {
    if (isAutoMode) return 'AUTO';
    if (isManualMode) return 'MANUAL';
    if (isStopMode) return 'STOP';
    return 'UNKNOWN';
  }
}

class RelayState {
  final int index;
  final String name;
  final bool state;

  RelayState({required this.index, required this.name, required this.state});

  factory RelayState.fromJson(Map<String, dynamic> json) {
    return RelayState(
      index: json['idx'] as int? ?? 0,
      name: json['name'] as String? ?? 'Relay',
      state: json['state'] == true,
    );
  }
}
