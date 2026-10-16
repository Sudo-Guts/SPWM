# SPWM — Inversor trifásico

Proyecto de control y etapa de potencia de un inversor trifásico con modulación sinusoidal por ancho de pulso (SPWM).

## Objetivo

Generar tres señales de control con desfases de 120 grados mediante un microcontrolador ATmega328P, para gobernar las tres ramas de un inversor trifásico mediante drivers de compuerta.

## Hardware actual

- Microcontrolador: ATmega328P.
- Cristal en el esquema: 16 MHz. Los fusibles y la frecuencia efectiva aún deben verificarse.
- Drivers: tres IR2109.
- Etapa de potencia: seis MOSFET. El esquema identifica IRF540N; el componente físico previsto es IRF640N. Es necesario comprobar compatibilidad de pinout, huella, manejo de compuerta y especificaciones antes de validar la placa.
- Alimentación de potencia prevista: 28 V, fuente externa limitada.

## Estructura

```text
SPWM/
├── .gitignore
├── README.md
├── C0DE/
│   ├── Makefile
│   ├── inc/
│   ├── src/
│   └── out/
├── DOC/
│   └── CheckList.txt
├── PCB/
│   ├── SPWM.kicad_pro
│   ├── SPWM.kicad_sch
│   ├── SPWM.kicad_pcb
│   └── fabrication/
└── SIM/
    ├── psim/
    └── spice/
        └── models/
```

- **C0DE/**: firmware y compilación con AVR-GCC y GNU Make.
- **DOC/**: seguimiento del proyecto mediante `CheckList.txt`, tareas por etapas y criterios de validación.
- **PCB/**: proyecto editable de KiCad; `fabrication/` contiene archivos de salida existentes, no aprobados todavía para fabricar.
- **SIM/psim/**: modelos y circuitos de simulación en PSIM.
- **SIM/spice/**: simulaciones eléctricas SPICE; `models/` guarda los modelos de componentes utilizados.

## Estado del borrador

El firmware incluido es solo un punto de partida que permanece sin PWM ni habilitación de los drivers. No representa la implementación SPWM final. Los archivos de KiCad se recuperaron del proyecto existente en Drive. Falta comprobar ERC/DRC, huellas, pads, asignación de pines, conexión de potencia y sincronización PWM antes de usar la placa o los Gerbers para fabricar.

## Compilación (cuando se disponga de Linux + avr-gcc)

```sh
cd C0DE
make
make size
make clean
```

El Makefile supone F_CPU = 16 MHz para la compilación; verificar primero la fuente de reloj real y los fusibles del ATmega328P.
