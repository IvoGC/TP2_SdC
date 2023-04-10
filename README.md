# TP2_SdC
Trabajo practico 2 de sistemas de Computacion

# Aplicación de conversión de cripto monedas
Se realizaron dos aplicaciones para la conversión de cripto monedas, en los directorios `python` y `c` se podrán encontrar las correspondientes aplicaciones.

![Repositorio del proyecto](https://github.com/IvoGC/TP2_SdC)

## Aplicación Python
La aplicación está desarrollada en tres capas. La capa más alta está escrita en python y se encarga de la request a la API y de la presentación de los datos a través de una interfaz gráfica.

Esta aplicación de python hace uso de una librería dinámica generada con código en C y en assembler. Para multiplicar el dato que ingresa el usuario por los resultados obtenidos de la request a la API.

Para esto, inicialmente es necesario generar la librería dinámica, los comandos utilizados se encuentran en el MakeFile del proyecto

```Makefile
$(lib)/librequests.so: $(out)/requests.o $(out)/mult.o
	mkdir -p $(lib)/
	gcc $(CFLAGS) -shared -o $@ $^

$(out)/requests.o: $(src)/requests.c
	mkdir -p $(out)/
	gcc $(CFLAGS) -o $@ -c -fPIC $^ 

$(out)/mult.o: $(src)/mult.asm
	nasm -f elf64 -o $@ $^
```

Luego, en el directorio `./lib/` encontraremos la librería .so.
Notese que para poder realizar esta librería dinámica fue necesario compilar (y escribir) el código de assembler de forma compatible con la arquitectura x86-64.

Para incluirla en el código de python, se hace uso de la librería `ctypes` para esto es necesario ejecutar el comando `import ctypes`.

```python
class Prices(ctypes.Structure):
    _fields_ = [
        ('usd', ctypes.c_float),
        ('euros', ctypes.c_float),
        ('pesos', ctypes.c_float),
    ]

librequests = ctypes.CDLL("./lib/librequests.so")
librequests.getPrices.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float)
librequests.getPrices.restype = Prices
```
Fue necesario definir una clase (que representa una estructura de C) ya que la función retorna una struc con floats.

Si ahora analizamos el código de C implementado en el archivo `requests.c`, este simplemente genera una estructura del tipo Prices (que será lo que se retorna a python) y luego inicializa todos sus campos mediante la función mult que recibe dos floats y retorna el producto entre ellos.
En realidad, la operación específica de multiplicación de los números flotantes se realiza en una subrutina que se implementó en código ensamblador.

En este caso no se utiliza la convención de llamadas vista en clases, si no que se utiliza la convención de x86-64. En la cual, los parámetros que son pasados a la función no están en la pila si no que en registros del coprocesador. En el caso de los números flotantes, los parámentros se encuentran en los registros xmm0 y xmm1 (si fuesen más en xmm2 y así). Y los resultados de punto flotante son retornados en el registro xmm0.

https://learn.microsoft.com/es-es/cpp/build/x64-calling-convention?view=msvc-170

Por lo tanto si analizamos el código en assembler, este es demasiado simple y no requiere ningún manejo del stack frame.

```assembly
segment .text

        global asm_mult
asm_mult:
                    
               
        mulss           xmm0, xmm1
        
        ret
```
La operación `mulss` multiplica dos valores de punto flotante, y como el resultado queda en xmm0, ya se puede salir de la función ya que allí estará presente el resultado que se necesita en la función de C.

![image](https://user-images.githubusercontent.com/83674694/230092311-d3325967-de44-4d06-a12f-cabf60d0174c.png)

## Aplicación C
Esta aplicación está desarrollada solo en lenguaje C y assembler para una subrutina específica (multipĺicación). El código `apiRequest.c` hace uso de la librería `-libcurl` para realizar la consulta a la api. Una vez realizada la consulta coloca en su standar output el resultado, luego el otro programa hará uso de esta salida.

Para poder obtener estos resultados, el segundo código `converter.c` realiza un fork para ejecutar el binario anterior, y mediante un pipe con su hijo, obtiene la salida. Esta salida es utilizada para obtener la conversión del número ingresado por el usuario.
Si analizamos el makefile del proyecto podemos observar que se generaran dos archivos binarios, el que efectivamente lanzará la aplicación es `converter`

```Makefile
CFLAGS = -g -pedantic 
include = ./include
src = ./src
out = ./out
lib = ./lib
bin = ./bin

all: api converter

api: $(bin)/apiRequest
$(bin)/apiRequest: $(src)/apiRequest.c
	mkdir -p $(bin)
	gcc $(CFLAGS) -o $@ $^ -lcurl -lcjson

converter: $(bin)/converter 
$(bin)/converter: $(src)/converter.c $(out)/mult.o $(out)/asm_io.o
	gcc $(CFLAGS) -o $@ -m32 $^

$(out)/mult.o: $(src)/mult.asm
	mkdir -p $(out)/
	nasm -f elf32 -o $@ $^

$(out)/asm_io.o: $(src)/asm_io.asm
	nasm -f elf32 -d ELF_TYPE -o $@ $^

clean:
	rm -rf $(bin)/ $(out)/

```
Podemos ver que la subrutina de multiplicación se encuentra definida en el código fuente `mult.o`, la función se llama `asm_mul` y es llamada desde el `converter.c` tres veces. La subrutina es la siguiente: 

```assembly
%include "./include/asm_io.inc"

segment .data

segment .bss

segment .text

        global asm_mult

asm_mult:

        enter           4,0
        
        mov             dword [ebp-4], 0
        mov             eax, [ebp+8]
        mov             ebx, [ebp+12]
        imul            ebx, eax
        mov             [ebp-4], ebx
        mov             eax, ebx
        dump_stack      1, 1, 3

        leave        
        ret
```
En la misma se hace un uso innecesario de variables locales, el único propósito es para visualizar como se comporta el stackFrame. Se utilizan funcionalidades definidas en `asm_io.asm` para imprimir el stack y poder analizarlo.

Podemos analizar una salida de ejecución de la aplicación

![image](https://user-images.githubusercontent.com/83674694/230749252-5540a20a-68fd-466d-a00f-82873b89ca09.png)

Aquí puede notarse como coinciden los números presentes en el stack con las salidas de la función.

## Análisis del stack frame de una aplicación en Assembly
Para terminar de comprender el comportamiento del stackFrame en una aplicación, se realizó el siguiente análisis de un programa de ejemplo de Paul Carter.

```C
#include <stdio.h>

#include "cdecl.h"

void PRE_CDECL calc_sum( int, int * ) POST_CDECL; /* prototype for assembly routine */

int main( void )
{
  int n, sum;

  printf("Sum integers up to: ");
  scanf("%d", &n);
  calc_sum(n, &sum);
  printf("Sum is %d\n", sum);
  return 0;
}
```
```assembly
segment .text
        global  calc_sum
;
; local variable:
;   sum at [ebp-4]
calc_sum:
        enter   4,0               ; allocate room for sum on stack
        push    ebx               ; IMPORTANT!

        mov     dword [ebp-4],0   ; sum = 0
        mov     ecx, 1            ; ecx is i in pseudocode
for_loop:
        cmp     ecx, [ebp+8]      ; cmp i and n
        jnle    end_for           ; if not i <= n, quit

        add     [ebp-4], ecx      ; sum += i
        inc     ecx
        jmp     short for_loop

end_for:
        mov     ebx, [ebp+12]     ; ebx = sump
        mov     eax, [ebp-4]      ; eax = sum
        mov     [ebx], eax

        pop     ebx               ; restore ebx
        leave
        ret
```
Para poder analizarlo con gdb, compilaremos los codigos con la bandera `-g` que agregará la información para el debug.

Al inicio, podemos observar que valores y que direcciones tienen las variables que serán pasadas como parámetro a la función de ensamblador. 

![Screenshot from 2023-04-05 09-19-40](https://user-images.githubusercontent.com/83674694/230094512-29769be0-ffba-4bfc-8761-c7c08ab09fa3.png)

En la siguiente imágen, ya se ingresó a la subrutina. Notese que el `ebp` aún no está referenciado al `esp`.

![Screenshot from 2023-04-05 09-20-03](https://user-images.githubusercontent.com/83674694/230094714-9e020882-3b71-415c-baab-9d7d900b5bff.png)

Una vez que se ejecute la función `enter` el `esp` se debería reducir en 0x8, ya que inicialmente pusheará a la pila el valor de `ebp`, luego igualará `ebp` a `esp` y finalmente restará 0x4 para hacer espacio para la variable local. `ebp` solo quedará con el valor del `esp` inicial menos 0x4.

```
In [6]: hex(0xffffcf0c - 0x08)
Out[6]: '0xffffcf04'
```

![Screenshot from 2023-04-05 09-20-28](https://user-images.githubusercontent.com/83674694/230095669-b8bed6e9-36bc-4c9b-8036-cf55b94e92d3.png)

La siguiente instrucción puede verse en la imágen, esta es `push ebx`, por lo tanto, es esperable que el valor del `esp` se decremente en 0x4.

![Screenshot from 2023-04-05 09-23-10](https://user-images.githubusercontent.com/83674694/230102211-72603ed6-a0f7-4dd8-83f7-904f1e0aea53.png)

Podemos hacer un análisis de la pila mediante el `ebp`

![Screenshot from 2023-04-05 09-30-49](https://user-images.githubusercontent.com/83674694/230097202-6e87cd86-c0a0-4dc9-a264-1bce44dbaa83.png)

en `ebp-4` tenemos el contenido de la variable local, que se encuentra en 0.

en `ebp` tenemos el valor de ebp pusheado al inicio del programa.

en `ebp+4` tenemos la dirección de retorno.

en `ebp+8` tenemos la variable n, que puede verse tiene un valor de 2.

en `ebp+16` tenemos la variable sum, que puede verse tiene un valor indiferente ya que todavía no se la utilizó.

La siguiente instrucción que se ejecuta es una comparación, el resultado de esto modificará el registro `eflags` que será utilizado por la función `jnle` para tomar una decisión.

La función de salto, realiza el salto si ZF = 0 y SF = OF

![Screenshot from 2023-04-05 09-33-59](https://user-images.githubusercontent.com/83674694/230098704-7b16f7a2-8b0b-4ad5-baba-4eba9ec1e0f4.png)

En este caso, podemos ver que ZF = 0, OF = 0 (porque no se encuentran en el listado) y SF = 1, por lo tanto no realiza el salto si no que omite la instrucción.

Lo siguiente que realiza es agregar el valor de `ecx` a la variable local `ebp-4` y luego incrementa el ecx y vuelve al loop.

![Screenshot from 2023-04-05 09-40-27](https://user-images.githubusercontent.com/83674694/230099211-6828345c-a99c-45a5-94d9-453db8854fea.png)

Luego, vuelve a realizar la comparación, y tomar la decisión, en este caso tampoco realiza el salto debido a que ZF = 1.

![Screenshot from 2023-04-05 09-41-06](https://user-images.githubusercontent.com/83674694/230099630-b53c20aa-71a2-4f83-8ac0-76bc6610e336.png)

Acumula el valor de `ecx` en la variable local y lo incrementa.

![Screenshot from 2023-04-05 09-42-26](https://user-images.githubusercontent.com/83674694/230099971-95fc29b5-a9a2-451f-b10a-de406149914d.png)

Vuelve a comparar, en este caso ZF = 0 Y SF = OF = 0 por lo tanto realiza el salto condicional

![Screenshot from 2023-04-05 09-43-13](https://user-images.githubusercontent.com/83674694/230100151-ab606193-a73f-421c-9b6b-9d66e09bc9d6.png)

Ahora, colocará en `ebx` el contenido de `[ebp+12]` que es la dirección de memoria de la variable sum. Luego coloca en `eax` el valor de `ebp-4` que es el valor de la variable local definida. Por ultimo en el contenido de `[ebx]` que es la dirección de memoria de sum, coloca el valor de `eax` que contiene el resultado final.

![Screenshot from 2023-04-05 09-46-24](https://user-images.githubusercontent.com/83674694/230100871-00ac9d75-ce2b-4654-9735-bc034c1d756b.png)

Luego, se realzia el pop de `ebx`, por lo que el `esp` se debería incrementar en 0x4

![Screenshot from 2023-04-05 09-48-35](https://user-images.githubusercontent.com/83674694/230101047-41a3703d-f9c1-4643-9e7d-e7af5eb9f96c.png)

Por último, al ejecutar `leave` se restaura el valor inicial del `ebp` que estaba guardado en la pila y `esp`nuevamente apunta a la dirección de retorno de la función anterior. En otras palabras, deja todo como estaba antes de `enter`.

![Screenshot from 2023-04-05 09-49-08](https://user-images.githubusercontent.com/83674694/230101298-75f87173-c042-4004-a09e-0da197ba7063.png)

Aquí se ve como se retorna a la función de C y se imprime el valor de sum, que fue modificado dentro de la función de assembler

![Screenshot from 2023-04-05 09-49-37](https://user-images.githubusercontent.com/83674694/230101549-8ef34d52-c63f-456e-96c7-ea590a7bf8ec.png)
