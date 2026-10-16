# =============================================================================
# Proyecto : Inversor Trifasico SPWM
# Archivo  : C0DE/Makefile
# Autor    : Gustavo Ruiz Luis
# =============================================================================
# Herramientas    : AVR-GCC, GNU Make y AVRDUDE
# Microcontrolador: ATmega328P, cristal externo de 16 MHz
# Programador     : USBasp mediante interfaz ISP
#
# Ejecutar "make help" para consultar los comandos disponibles.
#
# NOTAS:
#   - Todos los archivos generados se guardan en out/.
#   - El modo laboratorio depende de SPWM_BENCH_TEST.
#   - "make flash" programa Flash, pero NO modifica los fuse bits.
#   - Mantener desconectada la etapa de potencia durante las pruebas.
# =============================================================================

.DEFAULT_GOAL := all

# -----------------------------------------------------------------------------
# 1. Microcontrolador y modo de funcionamiento
# -----------------------------------------------------------------------------

MCU             := atmega328p
F_CPU           := 16000000UL
SPWM_BENCH_TEST ?= 0

# Modos:
#   0 = Funcionamiento normal controlado mediante UART.
#   1 = Arranque automatico para pruebas de laboratorio.
#
# Rechazar configuraciones diferentes de 0 o 1.

ifeq ($(filter $(SPWM_BENCH_TEST),0 1),)
$(error SPWM_BENCH_TEST debe ser 0 o 1)
endif

# -----------------------------------------------------------------------------
# 2. Herramientas de desarrollo
# -----------------------------------------------------------------------------

CC      := avr-gcc
OBJCOPY := avr-objcopy
OBJDUMP := avr-objdump
SIZE    := avr-size
AVRDUDE := avrdude

# -----------------------------------------------------------------------------
# 3. Programador ISP
# -----------------------------------------------------------------------------

# USBasp:
#   -c : Tipo de programador.
#   -p : Microcontrolador.
#   -B : Periodo de reloj ISP en microsegundos.
#
# El valor -B 20 fue probado con nuestro ATmega328P.

PROGRAMMER    := usbasp
AVR_PART      := m328p
BITCLOCK      := 20

AVRDUDE_FLAGS := -c $(PROGRAMMER) -p $(AVR_PART) -B $(BITCLOCK)

# -----------------------------------------------------------------------------
# 4. Configuracion de fuse bits
# -----------------------------------------------------------------------------

# Valores previamente programados y verificados.
#
# LFUSE = 0xFF:
#   Oscilador de cristal externo y CKDIV8 deshabilitado.
#   La frecuencia nominal de 16 MHz la proporciona el cristal instalado.
#
# HFUSE = 0xD9:
#   Configuracion conservada del microcontrolador.
#
# EFUSE = 0xFF:
#   Configuracion conservada del microcontrolador.
#
# IMPORTANTE:
#   fuses-set solamente escribe LFUSE.
#   HFUSE y EFUSE se verifican, pero no se modifican.
#   No cambiar el reloj sin comprobar primero el hardware.

LFUSE := 0xFF
HFUSE := 0xD9
EFUSE := 0xFF

# -----------------------------------------------------------------------------
# 5. Directorios y archivos generados
# -----------------------------------------------------------------------------

# Estructura:
#
# C0DE/
#   Makefile
#   inc/
#   src/
#   out/
#
# Todos los archivos de compilacion permanecen directamente en out/.

SRC_DIR := src
INC_DIR := inc
OUT_DIR := out
TARGET  := firmware

# Buscar automaticamente los archivos fuente .c.
SOURCES := $(wildcard $(SRC_DIR)/*.c)

# Convertir src/archivo.c en out/archivo.o.
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(SOURCES))

# Archivos de dependencias generados automaticamente por GCC.
DEPS := $(OBJECTS:.o=.d)

# Archivos finales.
ELF := $(OUT_DIR)/$(TARGET).elf
HEX := $(OUT_DIR)/$(TARGET).hex
MAP := $(OUT_DIR)/$(TARGET).map
LST := $(OUT_DIR)/$(TARGET).lst

# Archivo interno para detectar cambios de configuracion.
BUILD_CONFIG := $(OUT_DIR)/.build-config

# -----------------------------------------------------------------------------
# 6. Opciones de compilacion
# -----------------------------------------------------------------------------

# CPPFLAGS:
#   Define la frecuencia del microcontrolador.
#   Define el modo normal o laboratorio.
#   Indica el directorio de encabezados.

CPPFLAGS := -DF_CPU=$(F_CPU) \
            -DSPWM_BENCH_TEST=$(SPWM_BENCH_TEST) \
            -I$(INC_DIR)

# CFLAGS:
#   -mmcu                Arquitectura AVR seleccionada.
#   -std=gnu11           Estandar C11 con extensiones GNU.
#   -Os                  Optimizacion para reducir el codigo.
#   -g                   Informacion para depuracion.
#   -Wall -Wextra        Advertencias generales.
#   -Wshadow             Detecta variables que ocultan otras.
#   -Wconversion         Advierte conversiones implicitas.
#   -ffunction-sections  Seccion individual por funcion.
#   -fdata-sections      Seccion individual por dato.
#   -MMD -MP             Dependencias automaticas de encabezados.

CFLAGS := -mmcu=$(MCU) -std=gnu11 -Os -g \
          -Wall -Wextra -Wshadow -Wconversion \
          -ffunction-sections -fdata-sections \
          -MMD -MP

# LDFLAGS:
#   Selecciona el microcontrolador.
#   Elimina secciones sin utilizar.
#   Genera el mapa de memoria.

LDFLAGS := -mmcu=$(MCU) \
           -Wl,--gc-sections \
           -Wl,-Map,$(MAP)

# -----------------------------------------------------------------------------
# 7. Construccion del firmware
# -----------------------------------------------------------------------------

.PHONY: all help info clean size disasm flash
.PHONY: fuses-get fuses-verify fuses-set FORCE

# Objetivo predeterminado: generar firmware.hex.
all: $(HEX)

# Crear el directorio out/ si todavia no existe.
$(OUT_DIR):
	mkdir -p $@

# -----------------------------------------------------------------------------
# 7.1. Detectar cambios en las opciones de compilacion
# -----------------------------------------------------------------------------

# GNU Make normalmente compara fechas de archivos.
# No detecta automaticamente cambios en variables como SPWM_BENCH_TEST.
#
# Guardamos las opciones utilizadas en out/.build-config.
#
# Si la configuracion cambia:
#   1. Se actualiza .build-config.
#   2. Se recompilan los archivos objeto.
#   3. Se genera nuevamente el firmware.
#
# Si no cambia, se conservan los archivos ya compilados.

FORCE:

$(BUILD_CONFIG): FORCE | $(OUT_DIR)
	@printf '%s\n' \
		'MCU=$(MCU)' \
		'F_CPU=$(F_CPU)' \
		'SPWM_BENCH_TEST=$(SPWM_BENCH_TEST)' \
		'CPPFLAGS=$(CPPFLAGS)' \
		'CFLAGS=$(CFLAGS)' \
		'LDFLAGS=$(LDFLAGS)' > $@.tmp
	@cmp -s $@.tmp $@ && rm -f $@.tmp || mv -f $@.tmp $@

# -----------------------------------------------------------------------------
# 7.2. Compilar archivos fuente
# -----------------------------------------------------------------------------

# Ejemplo:
#   src/main.c -> out/main.o
#   src/pwm.c  -> out/pwm.o

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_CONFIG) | $(OUT_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# -----------------------------------------------------------------------------
# 7.3. Enlazar archivos objeto
# -----------------------------------------------------------------------------

# Genera:
#   out/firmware.elf
#   out/firmware.map

$(ELF): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

# -----------------------------------------------------------------------------
# 7.4. Generar Intel HEX
# -----------------------------------------------------------------------------

# Convierte el ELF a HEX para programar Flash.
# La memoria EEPROM se excluye deliberadamente.

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# -----------------------------------------------------------------------------
# 8. Analisis del firmware
# -----------------------------------------------------------------------------

# Mostrar memoria Flash y SRAM utilizada.
#
# Utiliza --format=avr porque nuestra version de avr-size
# no admite la opcion -C.

size: $(ELF)
	$(SIZE) --format=avr --mcu=$(MCU) $<

# Generar codigo ensamblador mezclado con el codigo C.
#
# Archivo generado:
#   out/firmware.lst

disasm: $(ELF)
	$(OBJDUMP) -d -S $< > $(LST)
	@echo "Listado generado: $(LST)"

# -----------------------------------------------------------------------------
# 9. Programacion de memoria Flash
# -----------------------------------------------------------------------------

# Programar el firmware utilizando USBasp.
#
# Este comando:
#   - Compila el firmware si es necesario.
#   - Programa la memoria Flash.
#   - Verifica la escritura mediante AVRDUDE.
#
# No escribe fuse bits.

flash: $(HEX)
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(HEX):i

# -----------------------------------------------------------------------------
# 10. Lectura y verificacion de fuse bits
# -----------------------------------------------------------------------------

# Leer los tres fuse bytes sin modificar el microcontrolador.

fuses-get:
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:r:-:h \
		-U hfuse:r:-:h \
		-U efuse:r:-:h

# Verificar los tres fuse bytes contra los valores configurados.

fuses-verify:
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:v:$(LFUSE):m \
		-U hfuse:v:$(HFUSE):m \
		-U efuse:v:$(EFUSE):m

# -----------------------------------------------------------------------------
# 11. Programacion controlada de fuse bits
# -----------------------------------------------------------------------------

# ADVERTENCIA:
#   Esta operacion modifica la configuracion del reloj.
#
# Requiere confirmacion explicita:
#
#   make fuses-set CONFIRM=YES
#
# Solamente escribe LFUSE.
# Despues verifica LFUSE, HFUSE y EFUSE.

fuses-set:
	@test "$(CONFIRM)" = "YES" || { \
		echo 'Operacion bloqueada: make fuses-set CONFIRM=YES'; \
		exit 1; \
	}
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:$(LFUSE):m
	$(MAKE) --no-print-directory fuses-verify

# -----------------------------------------------------------------------------
# 12. Informacion del proyecto
# -----------------------------------------------------------------------------

# Mostrar configuracion seleccionada sin compilar.

info:
	@echo 'MCU:         $(MCU)'
	@echo 'F_CPU:       $(F_CPU)'
	@echo 'Modo bench:  $(SPWM_BENCH_TEST) (0=UART, 1=laboratorio)'
	@echo 'Directorio:  $(OUT_DIR)/'
	@echo 'HEX:         $(HEX)'
	@echo 'Programador: $(PROGRAMMER), -B $(BITCLOCK)'

# -----------------------------------------------------------------------------
# 13. Ayuda
# -----------------------------------------------------------------------------

# Mostrar los comandos disponibles desde la terminal.

help:
	@echo 'Uso: desde C0DE/'
	@echo '  make                          Compilar firmware normal (UART)'
	@echo '  make SPWM_BENCH_TEST=1         Compilar firmware de laboratorio'
	@echo '  make flash                    Programar firmware normal'
	@echo '  make flash SPWM_BENCH_TEST=1   Programar firmware de laboratorio'
	@echo '  make size                     Consultar uso de Flash y SRAM'
	@echo '  make disasm                   Generar out/firmware.lst'
	@echo '  make info                     Mostrar configuracion actual'
	@echo '  make fuses-get                Leer fuse bits'
	@echo '  make fuses-verify             Verificar fuse bits'
	@echo '  make fuses-set CONFIRM=YES    Escribir LFUSE y verificar'
	@echo '  make clean                    Eliminar out/'

# -----------------------------------------------------------------------------
# 14. Limpieza
# -----------------------------------------------------------------------------

# Eliminar todos los archivos generados.
# No modifica los archivos fuente, encabezados ni fuse bits.

clean:
	rm -rf $(OUT_DIR)

# -----------------------------------------------------------------------------
# 15. Dependencias automaticas
# -----------------------------------------------------------------------------

# Los archivos .d permiten recompilar los objetos cuando se modifica
# cualquier archivo .h utilizado por el proyecto.
#
# El prefijo "-" evita errores si todavia no existen archivos .d.

-include $(DEPS)

