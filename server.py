from flask import Flask, request, jsonify
from datetime import datetime

app = Flask(__name__)

fridge_inventory = {}

@app.route("/", methods=["POST"])
def handle_sensor_data():
    try:
        # Retrieve JSON payload from the request
        data = request.get_json()
        if not data:
            return jsonify({"error": "No JSON payload provided"}), 400

        # Extract temperature, humidity, and RFID UID
        temperature = data.get("temperature")
        humidity = data.get("humidity")
        rfid_uid = data.get("rfid_uid")
        action = data.get("action", "status")  

        if temperature is not None and humidity is not None:
            
            print(f"[{datetime.now()}] Received: Temperature = {temperature}°C, Humidity = {humidity}%")

        if rfid_uid:
            if action == "add":
                item_name = data.get("item_name", "Unknown Item")
                fridge_inventory[rfid_uid] = {"name": item_name, "status": "Present"}
                print(f"[{datetime.now()}] Added {item_name} to inventory with UID {rfid_uid}.")
                return jsonify({"message": f"Item {item_name} added to inventory."}), 200
            elif action == "remove":
                if rfid_uid in fridge_inventory:
                    removed_item = fridge_inventory.pop(rfid_uid)
                    print(f"[{datetime.now()}] Removed {removed_item['name']} from inventory with UID {rfid_uid}.")
                    return jsonify({"message": f"Item {removed_item['name']} removed from inventory."}), 200
                else:
                    return jsonify({"error": "RFID UID not found in inventory."}), 404
            elif action == "toggle":
                if rfid_uid in fridge_inventory:
                    current_status = fridge_inventory[rfid_uid]["status"]
                    new_status = "Absent" if current_status == "Present" else "Present"
                    fridge_inventory[rfid_uid]["status"] = new_status
                    print(f"[{datetime.now()}] Toggled status of {fridge_inventory[rfid_uid]['name']} to {new_status}.")
                    return jsonify({"message": f"Status toggled to {new_status}."}), 200
                else:
                    return jsonify({"error": "RFID UID not found in inventory."}), 404
            else:
                return jsonify({"error": "Invalid action specified."}), 400

        return jsonify({"message": "Data received successfully."}), 200

    except Exception as e:
        print(f"Error: {e}")
        return jsonify({"error": str(e)}), 500


@app.route("/inventory", methods=["GET"])
def get_inventory():
    """
    """
    return jsonify(fridge_inventory), 200


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
