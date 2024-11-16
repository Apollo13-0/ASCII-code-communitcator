import socket
from client_aes import encrypt_file_content

def send_encrypted_file(input_path, server_ip, server_port):
    """
    Encrypts the content of a file and sends it to a server via a socket.
    """
    # Encrypt the content of the input file
    encrypted_content = encrypt_file_content(input_path)
    
    # Create a TCP/IP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connect to the server
        sock.connect((server_ip, server_port))
        print(f"Connected to server at {server_ip}:{server_port}")
        
        # Send the encrypted content
        sock.sendall(encrypted_content)
        print("Encrypted data sent to the server.")

# Example usage
# Specify the path to the file to be encrypted, and the server's IP and port
input_file_path = 'Client/quijote.txt'  # Path to the text file you want to encrypt
server_ip = '192.168.100.10'  # Replace with the server's IP address
server_port = 65432      # Replace with the server's port number

send_encrypted_file(input_file_path, server_ip, server_port)
