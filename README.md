# #uTinyOS - a micro tiny OS on FreeRTOS

---

> Un conjunto de tareas organizadas que despliegan un pequeño kernel capaz de administrar los recursos de un micro controlador.

#### Resumen

---

El proyecto consiste consiste en desplegar una estructura de componentes basados en los módulos nativos de ESP-IDF en forma de tareas a través de FreeRTOS. 

El kernel se reparte en cuatro componentes

- uTinyOS - Módulo que se encarga de desplegar todas las tareas necesarias para inicializar el sistema de forma correcta y la posible comunicación entre cada módulo.  

- uEventLoop - Módulo que define los eventos que los componentes utilizaran para controlar sus estados internos.

- uTaskManager - Abstrae el control de tareas otorgado por FreeRTOS.

- uModuleManager - Mantiene toda la información de los módulos implementados.

Luego, el resto de la estructura del proyecto, consiste en los módulos a desplegar.

```
uTinyOS/
├── partitions.csv
│
├── main/
│   ├── main.c
│   └── app_config.h
│
├── components/
│   │
│   ├── uTinyOS/            # Kernel que coordina las comunicaciones
│   ├── uEventLoop/         # Definicion de eventos para comunicación
│   ├── uTaskManager/       # Gestión de tareas FreeRTOS
│   ├── uModuleManager/     # Registro de módulos
│   ├── uWifiManager/       # Driver de WiFi
│   ├── uHttpServer/        # Driver del servidor
│   ├── uDriverNvs/         # Driver del almacenamiento no volátil
│   ├── uFileSystemSpiffs/  # Driver del FileSystem
│   ├── uDriverGpio/        # Driver de las GPIO
│   ├── uDriverSPI/         # Driver de comunicacion SPI
│   ├── uDriverI2c/         # Driver de comunicacion I2C
│   ├── uMdnsService/       # Gestor de servicios dns
│   ├── uConsole/           # Implementación de CLI
│   ├── uOtaManager/        # Gestor de actualización de firmware
│   └── uModuleRuntime/     # Manejo de programas agregados
│
└── spiffs/
    ├── index.html
    ├── style.css
    └── app.js
```

A futuro se buscará implementar un sistema de plug-ins donde, a través del módulo de OTA, se puedan cargar pequeños programas generados por la comunidad sin que afecte al normal funcionamiento del kernel desarrollado. Este sera implementado en el módulo `uModuleRuntime`

También se buscará generar una interfaz de comando de linea, en el módulo `uConsole`, para obtener informacion y dar ordenes al micro controlador sin la necesidad de interrumpir la ejecución de las tareas.

Se documentará módulo a módulo las implementaciones y sus funcionalidades. 



Es necesario configurar un archivo csv para especificar las particiones spiffs y OTA que vamos a utilizar. Además, a través de las configuraciones de IDF, establecemos el tamaño de la memoria del micro controlador.

Para diseño, implementaciones y desarrollo, se utilizará una micro controlador Arduino Nano con ESP32-s3 con encapsulado u-blox NORA-W106, 512 KB de SRAM, 384 KB de ROM, 8 MB de PSRAM y 16 MB de memoria Flash externa.

---

*@Arpone, David S. @Iloro, Gonzalo. @Lacourrage, Santiago. - 2026 - Licensed under: MIT*
