/// Data models for exercise schedule, fuel, maintenance, event log, etc.
library;

class ExerciseConfig {
  final bool enabled;
  final int day;
  final int hour;
  final int minute;
  final int durationMin;
  final bool loadTransfer;
  final String state;
  final String lastRun;
  final String? next;
  final int? remainingSec;

  ExerciseConfig({
    this.enabled = false,
    this.day = 1,
    this.hour = 10,
    this.minute = 0,
    this.durationMin = 15,
    this.loadTransfer = false,
    this.state = 'IDLE',
    this.lastRun = 'Never',
    this.next,
    this.remainingSec,
  });

  factory ExerciseConfig.fromJson(Map<String, dynamic> json) {
    return ExerciseConfig(
      enabled: json['enabled'] == true,
      day: json['day'] as int? ?? 1,
      hour: json['hour'] as int? ?? 10,
      minute: json['minute'] as int? ?? 0,
      durationMin: json['duration'] as int? ?? 15,
      loadTransfer: json['load_transfer'] == true,
      state: json['state'] as String? ?? 'IDLE',
      lastRun: json['last_run'] as String? ?? 'Never',
      next: json['next'] as String?,
      remainingSec: json['remaining'] as int?,
    );
  }

  Map<String, dynamic> toJson() => {
        'enabled': enabled,
        'day': day,
        'hour': hour,
        'minute': minute,
        'duration': durationMin,
        'load_transfer': loadTransfer,
      };
}

class FuelStatus {
  final bool hasSensor;
  final String fuelType;
  final String volUnit;
  final String rateUnit;
  final double tankSize;
  final double rateIdle;
  final double rateFull;
  final double effRate;
  final double loadPct;
  final double pct;
  final double remain;
  final double estHours;

  FuelStatus({
    this.hasSensor = false,
    this.fuelType = 'diesel',
    this.volUnit = 'L',
    this.rateUnit = 'lph',
    this.tankSize = 0,
    this.rateIdle = 0,
    this.rateFull = 0,
    this.effRate = 0,
    this.loadPct = 0,
    this.pct = 0,
    this.remain = 0,
    this.estHours = 0,
  });

  factory FuelStatus.fromJson(Map<String, dynamic> json) {
    return FuelStatus(
      hasSensor: json['sensor'] == true,
      fuelType: json['fuel_type'] as String? ?? 'diesel',
      volUnit: json['vol_unit'] as String? ?? 'L',
      rateUnit: json['rate_unit'] as String? ?? 'lph',
      tankSize: (json['tank_size'] as num?)?.toDouble() ?? 0,
      rateIdle: (json['rate_idle'] as num?)?.toDouble() ?? 0,
      rateFull: (json['rate_full'] as num?)?.toDouble() ?? 0,
      effRate: (json['eff_rate'] as num?)?.toDouble() ?? 0,
      loadPct: (json['load'] as num?)?.toDouble() ?? 0,
      pct: (json['pct'] as num?)?.toDouble() ?? 0,
      remain: (json['remain'] as num?)?.toDouble() ?? 0,
      estHours: (json['est_hours'] as num?)?.toDouble() ?? 0,
    );
  }
}

class EventEntry {
  final DateTime timestamp;
  final String message;

  EventEntry({required this.timestamp, required this.message});

  factory EventEntry.fromJson(Map<String, dynamic> json) {
    return EventEntry(
      timestamp: DateTime.fromMillisecondsSinceEpoch(
          ((json['ts'] as num?)?.toInt() ?? 0) * 1000),
      message: json['msg'] as String? ?? '',
    );
  }
}

class MaintenanceItem {
  final String name;
  final int intervalHours;
  final double lastResetHours;

  MaintenanceItem({
    required this.name,
    required this.intervalHours,
    required this.lastResetHours,
  });

  factory MaintenanceItem.fromJson(Map<String, dynamic> json) {
    return MaintenanceItem(
      name: json['name'] as String? ?? '',
      intervalHours: json['interval'] as int? ?? 250,
      lastResetHours: (json['reset'] as num?)?.toDouble() ?? 0,
    );
  }

  double remainingHours(double currentTotalHours) {
    return intervalHours - (currentTotalHours - lastResetHours);
  }
}

class BatteryHistory {
  final List<double> samples;
  final double min24h;
  final double max24h;
  final double avg24h;

  BatteryHistory({
    required this.samples,
    required this.min24h,
    required this.max24h,
    required this.avg24h,
  });

  factory BatteryHistory.fromJson(Map<String, dynamic> json) {
    final rawSamples = json['samples'] as List? ?? [];
    return BatteryHistory(
      samples: rawSamples.map((e) => (e as num).toDouble()).toList(),
      min24h: (json['min'] as num?)?.toDouble() ?? 0,
      max24h: (json['max'] as num?)?.toDouble() ?? 0,
      avg24h: (json['avg'] as num?)?.toDouble() ?? 0,
    );
  }
}

class RuntimeDay {
  final String date;
  final double hours;

  RuntimeDay({required this.date, required this.hours});

  factory RuntimeDay.fromJson(Map<String, dynamic> json) {
    return RuntimeDay(
      date: json['date'] as String? ?? '',
      hours: (json['hours'] as num?)?.toDouble() ?? 0,
    );
  }
}

class ConfigData {
  final int start;
  final int count;
  final List<int> data;

  ConfigData({required this.start, required this.count, required this.data});

  factory ConfigData.fromJson(Map<String, dynamic> json) {
    final rawData = json['data'] as List? ?? [];
    return ConfigData(
      start: json['start'] as int? ?? 0x42,
      count: json['count'] as int? ?? 0,
      data: rawData.map((e) => (e as num).toInt()).toList(),
    );
  }

  int? registerAt(int address) {
    final idx = address - start;
    if (idx < 0 || idx >= data.length) return null;
    return data[idx];
  }
}
