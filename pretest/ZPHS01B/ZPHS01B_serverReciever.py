from flask import Flask, request, jsonify
import json
from datetime import datetime

app = Flask(__name__)

@app.route('/api/airquality', methods=['POST'])
def receive_air_data():
    try:
        # JSON
        data = request.get_json()
        
        #get log
        with open('air_quality_log.json', 'a') as f:
            json.dump(data, f)
            f.write('\n')
        
        #printing
        print("\n" + "="*50)
        print(f"Received Air Quality Data | {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("="*50)
        
         
        print(f"Device ID: {data.get('device_id')}")
        print(f"RSSI: {data.get('rssi')} dBm | Calibrated: {data.get('calibrated')}")
        
         
        pm = data.get('pm', {})
        print("\nParticulate Matter:")
        print(f"  PM1.0: {pm.get('pm1_0')} μg/m³ [{pm.get('pm1_0_level')}]")
        print(f"  PM2.5: {pm.get('pm2_5')} μg/m³ [{pm.get('pm2_5_level')}]")
        print(f"  PM10:  {pm.get('pm10')} μg/m³ [{pm.get('pm10_level')}]")
        
        
        gases = data.get('gases', {})
        print("\nGases:")
        print(f"  CO₂:  {gases.get('co2')} ppm [{gases.get('co2_level')}]")
        print(f"  TVOC: {gases.get('tvoc')} level [{gases.get('tvoc_level')}]")
        print(f"  CH₂O: {gases.get('ch2o')} μg/m³ [{gases.get('ch2o_level')}]")
        print(f"  CO:   {gases.get('co')} ppm [{gases.get('co_level')}]")
        print(f"  O₃:   {gases.get('o3')} ppm [{gases.get('o3_level')}]")
        print(f"  NO₂:  {gases.get('no2')} ppm [{gases.get('no2_level')}]")
        
        
        env = data.get('environment', {})
        print("\nEnvironment:")
        print(f"  Temperature: {env.get('temperature')}°C")
        print(f"  Humidity:    {env.get('humidity')}% [{env.get('humidity_level')}]")
        
        print("\nData processed successfully!")
        return jsonify({"status": "success", "message": "Data received"}), 200
    
    except Exception as e:
        print(f"Error: {str(e)}")
        return jsonify({"status": "error", "message": str(e)}), 400


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
