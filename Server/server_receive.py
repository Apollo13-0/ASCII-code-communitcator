import socket

def receive_encrypted_file(output_path, server_ip, server_port):
    # Create a TCP/IP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((server_ip, server_port))
        server_socket.listen(1)
        
        print("Waiting for a connection...")
        connection, client_address = server_socket.accept()
        
        with connection:
            print(f"Connected to {client_address}")
            
            # Receive the file size first
            file_size_bytes = connection.recv(4)
            file_size = int.from_bytes(file_size_bytes, 'big')
            print(f"Expecting {file_size} bytes")

            # Receive the encrypted content in chunks
            received_data = b''
            while len(received_data) < file_size:
                chunk = connection.recv(4096)
                if not chunk:
                    break
                received_data += chunk

            # Save the encrypted content to a file
            with open(output_path, 'wb') as output_file:
                output_file.write(received_data)
            
            print("Encrypted file received and saved to:", output_path)

def main():
    output_path = 'Server/encrypted_content_received.bin'  # Path to save the received encrypted file
    server_ip = '192.168.100.10'  # Change to the server's IP address
    server_port = 65432       # Server's port

    receive_encrypted_file(output_path, server_ip, server_port)

if __name__ == "__main__":
    main()
