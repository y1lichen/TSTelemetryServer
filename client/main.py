import socket
import json

HOST = "localhost"  # Change if necessary
PORT = 3101  # Plugin port


def listen_to_telemetry():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        print(f"Connected to {HOST}:{PORT}")

        buffer = ""  # Store partial JSON data

        while True:
            try:
                data = s.recv(4096).decode("utf-8")  # Receive and decode data
                if not data:
                    break

                buffer += data.replace("\x00", "")  # Remove null characters

                # Attempt to parse JSON if buffer contains a full JSON object
                try:
                    telemetry = json.loads(buffer)
                    print(json.dumps(telemetry, indent=4))
                    buffer = ""  # Reset buffer after successful parsing
                except json.JSONDecodeError:
                    pass  # Wait for more data if JSON is incomplete

            except KeyboardInterrupt:
                print("\nDisconnected from server")
                break


if __name__ == "__main__":
    listen_to_telemetry()

