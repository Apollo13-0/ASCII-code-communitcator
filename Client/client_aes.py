from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import padding
import os
import re  # Para manipular texto y reemplazar caracteres

# Static AES 256-bit (32 bytes) key
AES_KEY = b'\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20'  # Example key

def read_file(path):
    with open(path, 'r', encoding='utf-8') as file:
        content = file.read()
    return content

# Removes special characters and replaces periods with spaces
def clean_text_preserve_spaces(text):
    # Replace periods with spaces
    text = text.replace('.', ' ')
    text = text.replace('\n', ' ')
    # Remove all other non-alphanumeric characters except spaces
    return re.sub(r'[^a-zA-Z0-9 ]', '', text)

# Encrypts data using AES encryption in CBC mode with PKCS7 padding.
# Returns IV + encrypted data.
def aes_encrypt(data, key):
    iv = os.urandom(16)
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    
    # Add padding to the data to ensure it's a multiple of the block size
    padder = padding.PKCS7(algorithms.AES.block_size).padder()
    padded_data = padder.update(data) + padder.finalize()
    
    # Encrypt the padded data
    encrypted_data = encryptor.update(padded_data) + encryptor.finalize()
    
    # Return IV + encrypted data
    return iv + encrypted_data

# Reads the content of the input file, cleans special characters (except spaces), encrypts it, and returns the encrypted content.
def encrypt_file_content(input_path):
    content = read_file(input_path)
    cleaned_content = clean_text_preserve_spaces(content)  # Clean the content
    encrypted_content = aes_encrypt(cleaned_content.encode('utf-8'), AES_KEY)
    
    return encrypted_content
