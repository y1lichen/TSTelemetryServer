import socket
import json


HOST = "localhost"
PORT = 3101


def listen_to_telemetry():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        print(f"Connected to {HOST}:{PORT}")

        buffer = ""

        while True:
            try:
                data = s.recv(1024).decode("utf-8")
                if not data:
                    print("Connection closed by server.")
                    break

                buffer += data  # Append new data to buffer

                while True:  # Process multiple complete JSON frames
                    frame_start_index = buffer.find("{")  # Find start of JSON
                    frame_end_index = buffer.find('"payloadType":"frame"')

                    if frame_start_index == -1 or frame_end_index == -1:
                        break  # Wait for more data

                    # Find the last closing brace AFTER "payloadType":"frame"
                    last_bracket_index = buffer.find("}", frame_end_index)

                    if last_bracket_index == -1:
                        break  # Wait for more data

                    json_str = buffer[frame_start_index : last_bracket_index + 1]

                    try:
                        telemetry = json.loads(json_str)                        # Remove processed JSON frame from buffer
                        buffer = buffer[last_bracket_index + 1 :].strip()

                    except json.JSONDecodeError as e:
                        print(f"JSON decode error: {e} - Raw data: {json_str}")
                        break  # JSON is incomplete, wait for more data

            except KeyboardInterrupt:
                print("\nDisconnected from server")
                break
            except Exception as e:
                print(f"A general error occurred: {e}")
                break


if __name__ == "__main__":
    listen_to_telemetry()
