import socket
from client_aes import encrypt_file_content
import struct

def send_encrypted_file(input_path, server_ip, server_port):
    encrypted_content = encrypt_file_content(input_path)
    message_length = len(encrypted_content)

    # Prepara el mensaje con 4 bytes iniciales para la longitud
    message = struct.pack('!I', message_length) + encrypted_content

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((server_ip, server_port))
        sock.sendall(message)
        print(f"Encrypted data of {message_length} bytes sent to the server.")


# Example usage
# Specify the path to the file to be encrypted, and the server's IP and port
input_file_path = 'Client/quijote.txt'  # Path to the text file you want to encrypt
server_ip = '192.168.122.1'  # Replace with the server's IP address
server_port = 65432      # Replace with the server's port number

send_encrypted_file(input_file_path, server_ip, server_port)
