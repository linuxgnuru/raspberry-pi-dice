#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <time.h> // for srand

#include <wiringPi.h>

#define MAX_NUM 7
#define MAX_DIG 6

/*
  0   4
  1 3 5
  2   6
*/
const int pins[7] = { 0, 1, 2, 3, 4, 5, 6 };

const int pips[6][7] = {
 { 0, 0, 0, 1, 0, 0, 0 }, // 1
 { 1, 0, 0, 0, 0, 0, 1 }, // 2
 { 1, 0, 0, 1, 0, 0, 1 }, // 3
 { 1, 0, 1, 0, 1, 0, 1 }, // 4
 { 1, 0, 1, 1, 1, 0, 1 }, // 5
 { 1, 1, 1, 0, 1, 1, 1 }  // 6
};

const int inputPin = 7;
const int haltPin = 29;

static void die(int sig);
void roll_die();

int main(int argc, char **argv)
{
  int i;
  int dl = 0;

  srand((unsigned)time(NULL));
  // note: we're assuming BSD-style reliable signals here
  (void)signal(SIGINT, die);
  (void)signal(SIGHUP, die);
  if (wiringPiSetup () == -1)
  {
    (void)fprintf(stderr, "oops %d\n", errno);
    return 1;
  }
  for (i = 0; i < MAX_NUM; i++)
  {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
  pinMode(inputPin, INPUT);
  pullUpDnControl(inputPin, PUD_UP);
  pinMode(haltPin, INPUT);
  pullUpDnControl(haltPin, PUD_UP);
  /*
  for (i = 0; i < 6; i++)
  {
    printDie(i);
    delay(500);
  }
  */
  while (1)
  {
    if (digitalRead(haltPin) == LOW)
    {
      system("shutdown -h now");
    }
    if (digitalRead(inputPin) == LOW)
    {
      dl = 1;
      for (i = 0; i < 30; i++)
      {
        roll_die();
        delay(dl);
        dl += 5;
      }
    }
  }
  return 0;
}

static void die(int sig)
{
  int i;
  for (i = 0; i < MAX_NUM; i++)
    digitalWrite(pins[i], LOW);
  if (sig != 0 && sig != 2)
      (void)fprintf(stderr, "caught signal %d\n", sig);
  if (sig == 2)
      (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
  exit(0);
}

void roll_die()
{
  int i, r;

  r = rand() % MAX_DIG;
  for (i = 0; i < 7; i++)
    digitalWrite(pins[i], pips[r][i]);
}

