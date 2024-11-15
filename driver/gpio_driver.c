#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define GPIO_SERVO 12         // Número de GPIO para el servo
#define PWM_PERIOD 20000      // Periodo total de 20 ms (50 Hz)
#define HEX_MAX 15            // Máximo valor hexadecimal (0xF)
#define MIN_DUTY_CYCLE 500    // Duración del pulso para 0° (0x0)
#define MAX_DUTY_CYCLE 2500   // Duración del pulso para 180° (0xF)

static int servo_position = 0;     // Posición actual del servo (en hexadecimal)
static int major_number;           // Número mayor asignado dinámicamente
static struct class *servo_class;  // Estructura para la clase del dispositivo
static struct device *servo_device;// Estructura para el dispositivo

// Función para calcular el duty cycle en función de un valor hexadecimal
static int calculate_duty_cycle(int hex_value) {
    if (hex_value < 0 || hex_value > HEX_MAX) {
        printk(KERN_WARNING "Valor hexadecimal inválido: %d\n", hex_value);
        return -1;
    }
    return MIN_DUTY_CYCLE + ((MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) * hex_value / HEX_MAX);
}

// Función para mover el servo a una posición basada en un valor hexadecimal
static void set_servo_position(int hex_value) {
    int duty_cycle = calculate_duty_cycle(hex_value);
    if (duty_cycle == -1) {
        return; // No mover el servo si el valor es inválido
    }

    gpio_set_value(GPIO_SERVO, 1); // Alto
    udelay(duty_cycle);            // Duración del pulso
    gpio_set_value(GPIO_SERVO, 0); // Bajo
    udelay(PWM_PERIOD - duty_cycle); // Completar el periodo

    servo_position = hex_value;   // Actualizar la posición actual
    printk(KERN_INFO "Servo movido a posición 0x%x\n", hex_value);
}

// Función de escritura para recibir el valor hexadecimal desde el espacio de usuario
static ssize_t gpio_driver_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    char hex_char;

    if (copy_from_user(&hex_char, buffer, sizeof(char))) {
        return -EFAULT;
    }

    if (hex_char >= '0' && hex_char <= '9') {
        set_servo_position(hex_char - '0');
    } else if (hex_char >= 'A' && hex_char <= 'F') {
        set_servo_position(hex_char - 'A' + 10);
    } else {
        printk(KERN_WARNING "Caracter inválido: %c\n", hex_char);
        return -EINVAL;
    }

    return sizeof(char);
}

// Función de lectura para devolver la posición actual en hexadecimal
static ssize_t gpio_driver_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    char hex_position[3]; // Cadena para el valor hexadecimal actual (e.g., "0x0\n")
    int hex_position_len;

    if (*offset != 0) {
        return 0; // Fin de la lectura
    }

    hex_position_len = snprintf(hex_position, sizeof(hex_position), "0x%x\n", servo_position);
    
    if (copy_to_user(buffer, hex_position, hex_position_len)) {
        return -EFAULT;
    }

    *offset = hex_position_len; // Actualizar el offset
    return hex_position_len;
}

// Configurar las operaciones del archivo del dispositivo
static struct file_operations fops = {
    .write = gpio_driver_write,
    .read = gpio_driver_read,
};

// Inicialización del módulo
static int __init gpio_driver_init(void) {
    int ret;

    printk(KERN_INFO "Inicializando gpio_driver.\n");

    // Solicitar el GPIO para el servo
    ret = gpio_request(GPIO_SERVO, "servo_pin");
    if (ret) {
        printk(KERN_ERR "No se pudo solicitar el GPIO %d\n", GPIO_SERVO);
        return ret;
    }

    ret = gpio_direction_output(GPIO_SERVO, 0);
    if (ret) {
        printk(KERN_ERR "No se pudo configurar el GPIO %d como salida\n", GPIO_SERVO);
        gpio_free(GPIO_SERVO);
        return ret;
    }

    // Solicitar un número mayor dinámico para el dispositivo
    major_number = register_chrdev(0, "gpio_driver", &fops);
    if (major_number < 0) {
        printk(KERN_ERR "Error al registrar el dispositivo de caracteres\n");
        gpio_free(GPIO_SERVO);
        return major_number;
    }

    // Crear la clase del dispositivo
    servo_class = class_create(THIS_MODULE, "servo_class");
    if (IS_ERR(servo_class)) {
        unregister_chrdev(major_number, "gpio_driver");
        gpio_free(GPIO_SERVO);
        return PTR_ERR(servo_class);
    }

    // Crear el archivo de dispositivo
    servo_device = device_create(servo_class, NULL, MKDEV(major_number, 0), NULL, "gpio_driver");
    if (IS_ERR(servo_device)) {
        class_destroy(servo_class);
        unregister_chrdev(major_number, "gpio_driver");
        gpio_free(GPIO_SERVO);
        return PTR_ERR(servo_device);
    }

    printk(KERN_INFO "Driver del servo inicializado correctamente con número mayor %d.\n", major_number);
    return 0;
}

// Función de limpieza del módulo
static void __exit gpio_driver_exit(void) {
    printk(KERN_INFO "Desinstalando gpio_driver.\n");

    gpio_set_value(GPIO_SERVO, 0);
    gpio_free(GPIO_SERVO);

    // Eliminar el archivo de dispositivo y limpiar la clase
    device_destroy(servo_class, MKDEV(major_number, 0));
    class_destroy(servo_class);

    // Desregistrar el número mayor
    unregister_chrdev(major_number, "gpio_driver");

    printk(KERN_INFO "Driver gpio_driver desinstalado.\n");
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TRIVIAL");
MODULE_DESCRIPTION("Driver para controlar un servo con valores hexadecimales en Raspberry Pi");

