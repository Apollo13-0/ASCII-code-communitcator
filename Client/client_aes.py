from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import padding
import os

# Static AES 256-bit (32 bytes) key
AES_KEY = b'\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20'  # Example key

def read_file(path):
    """
    Reads the content of a text file given its path.
    """
    with open(path, 'r', encoding='utf-8') as file:
        content = file.read()
    return content

def aes_encrypt(data, key):
    """
    Encrypts data using AES encryption in CBC mode with PKCS7 padding.
    Returns IV + encrypted data.
    """
    iv = os.urandom(16)  # 128-bit IV for AES
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    
    # Add padding to the data to ensure it's a multiple of the block size
    padder = padding.PKCS7(algorithms.AES.block_size).padder()
    padded_data = padder.update(data) + padder.finalize()
    
    # Encrypt the padded data
    encrypted_data = encryptor.update(padded_data) + encryptor.finalize()
    
    # Return IV + encrypted data
    return iv + encrypted_data

def encrypt_file_content(input_path):
    """
    Reads the content of the input file, encrypts it, and returns the encrypted content.
    """
    # Read the content from the input file
    content = read_file(input_path).encode('utf-8')
    
    # Encrypt the content with AES
    encrypted_content = aes_encrypt(content, AES_KEY)
    
    return encrypted_content


with open('Client/encrypted_content.bin', 'wb') as file:
    file.write(encrypt_file_content('Client/quijote.txt'))