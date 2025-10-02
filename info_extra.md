# To Do
[ ] Start :+1:

# Estructura del proyecto esperada:
```
firmware/
├─ platformio.ini
├─ README.md
├─ lib/                          # Librerías externas
├─ info_extra.md                 # Documentación adicional y lista de tareas
├─ context/
│  └─ requirements.md            # Contexto, requisitos e instrucciones para LLM en caso de usarse.
├─ .github/                      # Configuraciones de GitHub
├─ include/
│  ├─ config.h                   
│  ├─ pins.h                    # Definición de pines
│  ├─ hal.h                     # Interfaz de abstracción de hardware
│  ├─ tasks.h                   # Declaraciones de tareas
│  └─ util.h                    # Funciones utilitarias
└─ src/
   ├─ main.cpp                   
   ├─ hal/                       
   │  ├─ hal_motors.cpp         
   │  ├─ hal_ultrasonic.cpp     
   │  ├─ hal_leds.cpp           
   │  ├─ hal_audio.cpp          
   │  └─ hal_lcd.cpp            
   ├─ tasks/                     
   │  ├─ task_drive.cpp         
   │  ├─ task_sensing.cpp       
   │  ├─ task_leds.cpp          
   │  ├─ task_ui.cpp            
   │  └─ task_audio.cpp        #
   └─ util/                      
      ├─ log.h                  
      ├─ filters.h              
      └─ timing.h               

```