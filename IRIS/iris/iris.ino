#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
#define DIRECCION_OLED 0x3C

const uint8_t PIN_SERVO = 9;
const int CENTRO = 90;
const int IZQUIERDA = 60;
const int DERECHA = 120;
const int PASO_SERVO_MS = 12;
const int MIRADA_MAX = 14;

const int OJO_ANCHO = 44;
const int OJO_ALTO = 44;
const int OJO_RADIO = 10;

Adafruit_SSD1306 pantalla(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);
Servo cuello;
int anguloActual = CENTRO;

int desplazamientoMirada() {
  return map(anguloActual, IZQUIERDA, DERECHA, -MIRADA_MAX, MIRADA_MAX);
}

void dibujarOjo(int ancho, int alto, int desplazamiento) {
  int radio = min(OJO_RADIO, alto / 2);
  pantalla.fillRoundRect(64 + desplazamiento - ancho / 2, 32 - alto / 2, ancho, alto, radio, SSD1306_WHITE);
}

void mostrarOjo(int ancho, int alto, int desplazamiento) {
  pantalla.clearDisplay();
  dibujarOjo(ancho, alto, desplazamiento);
  pantalla.display();
}

void parpadear() {
  int desp = desplazamientoMirada();
  for (int a = OJO_ALTO; a > 2; a -= 7) {
    mostrarOjo(OJO_ANCHO, a, desp);
  }
  mostrarOjo(OJO_ANCHO, 2, desp);
  delay(40);
  for (int a = 9; a < OJO_ALTO; a += 7) {
    mostrarOjo(OJO_ANCHO, a, desp);
  }
  mostrarOjo(OJO_ANCHO, OJO_ALTO, desp);
}

void moverCuello(int destino) {
  cuello.attach(PIN_SERVO);
  cuello.write(anguloActual);
  while (anguloActual != destino) {
    anguloActual += (destino > anguloActual) ? 1 : -1;
    cuello.write(anguloActual);
    mostrarOjo(OJO_ANCHO, OJO_ALTO, desplazamientoMirada());
    delay(PASO_SERVO_MS);
  }
  delay(100);
  cuello.detach();
}

void dibujarFeliz(int dy) {
  pantalla.clearDisplay();
  pantalla.fillCircle(64, 40 + dy, 24, SSD1306_WHITE);
  pantalla.fillCircle(64, 49 + dy, 17, SSD1306_BLACK);
  pantalla.fillRect(0, 44 + dy, 128, 30, SSD1306_BLACK);
  pantalla.display();
}

void feliz() {
  const int rebote[] = {0, -5, -8, -5, 0, -5, -8, -5, 0};
  for (uint8_t i = 0; i < sizeof(rebote) / sizeof(rebote[0]); i++) {
    dibujarFeliz(rebote[i]);
    delay(60);
  }
  delay(1000);
  mostrarOjo(OJO_ANCHO, OJO_ALTO, 0);
}

void dibujarSorpresa(int tam) {
  pantalla.clearDisplay();
  dibujarOjo(tam, tam, 0);
  pantalla.fillCircle(64, 32, tam / 6, SSD1306_BLACK);
  pantalla.display();
}

void sorpresa() {
  for (int t = OJO_ALTO; t <= 56; t += 4) {
    dibujarSorpresa(t);
    delay(15);
  }
  delay(1300);
  for (int t = 56; t >= OJO_ALTO; t -= 4) {
    dibujarSorpresa(t);
    delay(15);
  }
  mostrarOjo(OJO_ANCHO, OJO_ALTO, 0);
}

void dibujarSueno(int alto, int zetas) {
  pantalla.clearDisplay();
  dibujarOjo(OJO_ANCHO, alto, 0);
  pantalla.setTextColor(SSD1306_WHITE);
  if (zetas >= 1) {
    pantalla.setTextSize(1);
    pantalla.setCursor(96, 26);
    pantalla.print("z");
  }
  if (zetas >= 2) {
    pantalla.setTextSize(2);
    pantalla.setCursor(106, 8);
    pantalla.print("Z");
  }
  pantalla.display();
}

void sueno() {
  for (int a = OJO_ALTO; a >= 12; a -= 4) {
    mostrarOjo(OJO_ANCHO, a, 0);
    delay(30);
  }
  dibujarSueno(12, 0);
  delay(500);
  dibujarSueno(12, 1);
  delay(600);
  dibujarSueno(12, 2);
  delay(700);
  for (int a = 12; a >= 3; a -= 3) {
    dibujarSueno(a, 2);
    delay(40);
  }
  dibujarSueno(2, 2);
  delay(800);
  for (int a = 9; a < OJO_ALTO; a += 6) {
    mostrarOjo(OJO_ANCHO, a, 0);
    delay(20);
  }
  mostrarOjo(OJO_ANCHO, OJO_ALTO, 0);
  delay(300);
  parpadear();
}

void mirarLado(int destino) {
  moverCuello(destino);
  delay(random(900, 1800));
  if (random(100) < 40) parpadear();
  moverCuello(CENTRO);
}

void reposo(unsigned long ms) {
  mostrarOjo(OJO_ANCHO, OJO_ALTO, desplazamientoMirada());
  delay(ms);
  parpadear();
  if (random(100) < 25) {
    delay(120);
    parpadear();
  }
}

void setup() {
  Wire.begin();
  if (!pantalla.begin(SSD1306_SWITCHCAPVCC, DIRECCION_OLED)) {
    for (;;);
  }
  randomSeed(analogRead(A0));
  moverCuello(CENTRO);
  mostrarOjo(OJO_ANCHO, OJO_ALTO, 0);
}

void loop() {
  static int ultima = -1;
  const uint8_t pesos[] = {0, 0, 1, 1, 2, 3, 4};

  reposo(random(1500, 3500));

  int accion;
  do {
    accion = pesos[random(sizeof(pesos))];
  } while (accion == ultima);
  ultima = accion;

  switch (accion) {
    case 0: mirarLado(IZQUIERDA); break;
    case 1: mirarLado(DERECHA); break;
    case 2: feliz(); break;
    case 3: sorpresa(); break;
    case 4: sueno(); break;
  }
}
