#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define GPIO_SERVO 12         // Número de GPIO para el servo
#define PWM_0_DEGREES 500     // Duración del pulso para 0°
#define PWM_90_DEGREES 1500   // Duración del pulso para 90°
#define PWM_180_DEGREES 2500  // Duración del pulso para 180°
#define PWM_PERIOD 20000      // Periodo total de 20 ms (50 Hz)

static int servo_angle = 0;         // Ángulo inicial
static int major_number;            // Número mayor asignado dinámicamente
static struct class *servo_class;   // Estructura para la clase del dispositivo
static struct device *servo_device; // Estructura para el dispositivo

// Función para establecer la posición del servo
static void set_servo_position(int angle) {
    int duty_cycle;

    if (angle == 0) {
        duty_cycle = PWM_0_DEGREES;
    } else if (angle == 90) {
        duty_cycle = PWM_90_DEGREES;
    } else if (angle == 180) {
        duty_cycle = PWM_180_DEGREES;
    } else if (angle >= 0 && angle <= 180) {
        // Interpolación simple entre 0 y 180 grados
        duty_cycle = PWM_0_DEGREES + ((angle * (PWM_180_DEGREES - PWM_0_DEGREES)) / 180);
    } else {
        printk(KERN_WARNING "Ángulo inválido: %d\n", angle);
        return;
    }

    // Almacenar el ángulo actual
    servo_angle = angle;

    // Generar el pulso PWM
    gpio_set_value(GPIO_SERVO, 1); // Alto
    udelay(duty_cycle);            // Duración del pulso
    gpio_set_value(GPIO_SERVO, 0); // Bajo
    udelay(PWM_PERIOD - duty_cycle); // Completar el periodo de 20 ms
}

// Función de escritura para recibir el ángulo desde el espacio de usuario
static ssize_t gpio_driver_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    int angle;

    if (copy_from_user(&angle, buffer, sizeof(int))) {
        return -EFAULT;
    }
    printk(KERN_INFO "Ángulo recibido: %d\n", angle);
    set_servo_position(angle);

    return sizeof(int);
}

// Función de lectura para devolver el ángulo actual del servo al espacio de usuario
static ssize_t gpio_driver_read(struct file *file, char __user *buffer, size_t len, loff_t *offset) {
    char angle_str[10];
    int angle_str_len;

    if (*offset != 0) {
        return 0; // Fin de la lectura
    }

    angle_str_len = snprintf(angle_str, sizeof(angle_str), "%d\n", servo_angle);
    
    if (copy_to_user(buffer, angle_str, angle_str_len)) {
        return -EFAULT;
    }

    *offset = angle_str_len; // Actualizar el offset para indicar que se ha leído todo
    return angle_str_len;
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
MODULE_AUTHOR("Tu Nombre");
MODULE_DESCRIPTION("Driver para controlar un servo motor usando GPIO en una Raspberry Pi y leer el ángulo actual");

