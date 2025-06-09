# 🏠 Home Assistant Integration

Comprehensive guide for integrating the Donation Box LED Controller with Home Assistant using MQTT.

## 🎯 Overview

The donation box publishes real-time data to MQTT topics that Home Assistant can consume to:
- **Track donations** in real-time with timestamps
- **Monitor LED modes** and automatic switching
- **Check system health** with heartbeat monitoring
- **Create automations** based on donation events
- **Display dashboards** with donation statistics

## 📋 Prerequisites

### Home Assistant Requirements
- Home Assistant with MQTT integration enabled
- MQTT broker (Mosquitto recommended)
- MQTT Explorer (optional, for debugging)

### Donation Box Requirements
- Donation box firmware with MQTT enabled (`ENABLE_MQTT = 1` in Config.h)
- WiFi connection configured
- MQTT broker credentials set in `credentials.h`

## 🔧 MQTT Broker Setup

### Option 1: Mosquitto Add-on (Recommended)
1. Install Mosquitto broker add-on in Home Assistant
2. Configure users and passwords
3. Note the broker IP and port (usually `homeassistant.local:1883`)

### Option 2: External MQTT Broker
If using an external broker, ensure it's accessible from both Home Assistant and the donation box.

## 📡 MQTT Topics Structure

The donation box publishes to these topics:

```
donation-box/{clientId}/donations    # Donation events
donation-box/{clientId}/mode         # LED mode changes  
donation-box/{clientId}/heartbeat    # System health (every 30s)
```

**Example with clientId `box-12345`:**
```
donation-box/box-12345/donations
donation-box/box-12345/mode
donation-box/box-12345/heartbeat
```

### Topic Payloads

#### Donations Topic
```json
{
  "timestamp": "2025-06-09T14:30:15Z",
  "mode": "Static Breathing",
  "amount": 1,
  "event": "donation"
}
```

#### Mode Topic
```json
{
  "timestamp": "2025-06-09T14:30:15Z", 
  "from_mode": "Static Breathing",
  "to_mode": "Wave Motion",
  "event": "mode_change"
}
```

#### Heartbeat Topic
```json
{
  "timestamp": "2025-06-09T14:30:15Z",
  "uptime": 1847392,
  "free_heap": 234567,
  "wifi_rssi": -45,
  "status": "online"
}
```

## 🏠 Home Assistant Configuration

### 1. MQTT Sensors Configuration

Add to your `configuration.yaml`:

```yaml
# Donation Box MQTT Sensors
mqtt:
  sensor:
    # Donation Counter
    - name: "Donation Box - Total Donations"
      state_topic: "donation-box/+/donations"
      value_template: "{{ value_json.amount | default(0) }}"
      unit_of_measurement: "donations"
      device_class: "monetary"
      
    # Current LED Mode
    - name: "Donation Box - Current Mode"
      state_topic: "donation-box/+/mode"
      value_template: "{{ value_json.to_mode | default('Unknown') }}"
      
    # System Health
    - name: "Donation Box - Status"
      state_topic: "donation-box/+/heartbeat"
      value_template: "{{ value_json.status | default('offline') }}"
      
    # WiFi Signal Strength
    - name: "Donation Box - WiFi Signal"
      state_topic: "donation-box/+/heartbeat"
      value_template: "{{ value_json.wifi_rssi | default(0) }}"
      unit_of_measurement: "dBm"
      device_class: "signal_strength"
      
    # System Uptime
    - name: "Donation Box - Uptime"
      state_topic: "donation-box/+/heartbeat"
      value_template: "{{ (value_json.uptime | default(0) / 3600) | round(1) }}"
      unit_of_measurement: "hours"
      
    # Free Memory
    - name: "Donation Box - Free Memory"
      state_topic: "donation-box/+/heartbeat"
      value_template: "{{ (value_json.free_heap | default(0) / 1024) | round(1) }}"
      unit_of_measurement: "KB"

  # Binary Sensors
  binary_sensor:
    # Online/Offline Status
    - name: "Donation Box - Online"
      state_topic: "donation-box/+/heartbeat"
      value_template: "{{ 'ON' if value_json.status == 'online' else 'OFF' }}"
      device_class: "connectivity"
      
  # Event Triggers
  device_automation:
    # Donation Event Trigger
    - trigger:
        platform: mqtt
        topic: "donation-box/+/donations"
      action:
        service: notify.mobile_app_your_phone
        data:
          message: "💰 New donation received! Mode: {{ trigger.payload_json.mode }}"
```

### 2. Template Sensors for Statistics

Add to `configuration.yaml`:

```yaml
template:
  sensor:
    # Daily Donation Counter
    - name: "Donations Today"
      state: >
        {{ states.sensor.donation_box_total_donations.state | int }}
      attributes:
        last_donation: >
          {% set donations = states | selectattr('entity_id', 'match', 'sensor.donation_box_.*') 
                                   | selectattr('attributes.last_changed')
                                   | map(attribute='attributes.last_changed') | list %}
          {% if donations %}
            {{ donations | max }}
          {% else %}
            Never
          {% endif %}
            
    # Donation Rate (per hour)
    - name: "Donation Rate"
      state: >
        {% set uptime_hours = states('sensor.donation_box_uptime') | float %}
        {% set total_donations = states('sensor.donation_box_total_donations') | int %}
        {% if uptime_hours > 0 %}
          {{ (total_donations / uptime_hours) | round(2) }}
        {% else %}
          0
        {% endif %}
      unit_of_measurement: "donations/hour"
```

### 3. Automation Examples

#### Donation Notification
```yaml
automation:
  - alias: "Donation Box - New Donation Alert"
    trigger:
      platform: mqtt
      topic: "donation-box/+/donations"
    action:
      - service: notify.family_group
        data:
          title: "💰 Donation Received!"
          message: >
            A new donation was detected in {{ trigger.payload_json.mode }} mode
            at {{ trigger.payload_json.timestamp | as_datetime | as_local }}
      - service: light.turn_on
        target:
          entity_id: light.living_room_lamp
        data:
          color_name: "green"
          brightness: 255
      - delay: "00:00:03"
      - service: light.turn_off
        target:
          entity_id: light.living_room_lamp

  - alias: "Donation Box - Offline Alert"
    trigger:
      platform: state
      entity_id: binary_sensor.donation_box_online
      to: "off"
      for: "00:02:00"  # 2 minutes offline
    action:
      service: notify.admin
      data:
        title: "⚠️ Donation Box Offline"
        message: "The donation box has been offline for 2 minutes. Check power and WiFi connection."

  - alias: "Donation Box - Daily Summary"
    trigger:
      platform: time
      at: "23:59:00"
    action:
      service: notify.family_group
      data:
        title: "📊 Daily Donation Summary"
        message: >
          Today's donations: {{ states('sensor.donations_today') }}
          Average rate: {{ states('sensor.donation_rate') }} donations/hour
          Current mode: {{ states('sensor.donation_box_current_mode') }}
```

## 📊 Dashboard Configuration

### Lovelace Card Examples

#### Overview Card
```yaml
type: entities
title: Donation Box Status
entities:
  - entity: sensor.donation_box_current_mode
    name: Current LED Mode
    icon: mdi:led-strip-variant
  - entity: sensor.donations_today
    name: Donations Today
    icon: mdi:hand-coin
  - entity: binary_sensor.donation_box_online
    name: System Status
  - entity: sensor.donation_box_wifi_signal
    name: WiFi Signal
```

#### Statistics Card
```yaml
type: statistics-graph
title: Donation Trends
entities:
  - sensor.donations_today
period: day
stat_types:
  - sum
  - mean
```

#### Gauge Card for Signal Strength
```yaml
type: gauge
entity: sensor.donation_box_wifi_signal
min: -100
max: -30
severity:
  green: -50
  yellow: -70
  red: -100
```

## 🔍 Debugging & Testing

### MQTT Explorer Setup
1. Install MQTT Explorer
2. Connect to your MQTT broker
3. Subscribe to `donation-box/+/+` to see all messages
4. Test donation detection by triggering the sensor

### Home Assistant MQTT Integration
1. Go to Settings → Devices & Services
2. Add MQTT integration if not already configured
3. Use "Listen to a topic" in Developer Tools to test:
   ```
   donation-box/+/donations
   donation-box/+/mode
   donation-box/+/heartbeat
   ```

### Troubleshooting

#### No MQTT Messages
- Check MQTT broker connection in donation box logs
- Verify WiFi connectivity
- Confirm MQTT credentials in `credentials.h`
- Check `ENABLE_MQTT = 1` in Config.h

#### Sensors Not Updating
- Restart Home Assistant after configuration changes
- Check MQTT integration logs in Home Assistant
- Verify topic names match exactly (case-sensitive)

#### Automation Not Triggering
- Test automation manually in Developer Tools
- Check automation traces for errors
- Verify trigger conditions are met

## 🎛️ Advanced Configurations

### Node-RED Integration
For more complex automations, use Node-RED:

```json
[{"id":"donation-flow","type":"mqtt in","topic":"donation-box/+/donations","broker":"mqtt-broker"}]
```

### InfluxDB & Grafana
Store long-term donation statistics:

```yaml
influxdb:
  include:
    entities:
      - sensor.donation_box_total_donations
      - sensor.donation_box_wifi_signal
      - sensor.donation_box_uptime
```

### Telegram Bot Integration
```yaml
telegram_bot:
  - platform: polling
    api_key: "YOUR_BOT_TOKEN"
    
notify:
  - platform: telegram
    name: donation_alerts
    chat_id: YOUR_CHAT_ID
```

## 📈 Monitoring Best Practices

1. **Set up offline alerts** to know when the system is down
2. **Monitor WiFi signal** to prevent connectivity issues
3. **Track donation patterns** to optimize placement
4. **Regular backups** of Home Assistant configuration
5. **Test automations** periodically to ensure they work

## 🔐 Security Considerations

- Use strong MQTT passwords
- Enable TLS for MQTT if possible
- Isolate IoT devices on separate network
- Regular firmware updates for donation box
- Monitor for unusual MQTT traffic

---

**🏠 Ready to integrate!** Follow this guide step by step, and your donation box will be fully integrated with Home Assistant for comprehensive monitoring and automation.
