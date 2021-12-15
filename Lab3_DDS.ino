/*Lab3_Demo1.ino
 * @file   Lab3_DDS.ino
 *   @author    Clayden Boyd, Jin Terada White
 *   @date      19-May-2021
 *   @brief  This sketch creates a DDS for two tasks:
 *     
 */
#include "TCB.h"
//task1 setup
#define LED                PORTB
#define LEDDDR             DDRB
#define LEDBIT             7 
//task2 setup
#define SPEAKER            OCR4A
#define SPEAKDDR           DDRH
#define SPEAKBIT           3 
#define PRESCALE          (1<<CS42) // 62.5 kHz
#define CLK_FREQ          62500
#define TIMER4_CTC_A_SET   (1<<COM4A0)
#define TIMER4_CTC_A_CLR   (1<<COM4A1) | 3
//===== “Close Encounters of the Third Kind” ====
#define D4 106 //293 // hz 
#define E4 94 //329 // hz 
#define C4 119 //261 // hz  
#define C3 239 //130 // hz  
#define Q4 158 //196 // hz  
#define REST  0
#define TASK_2_LENGTH 4800
int melody[] = {D4, REST, E4, REST, C4, REST, C3, REST, Q4, REST};
int rhythm[] = {300, 100, 400, 100, 400, 100, 400, 100, 800, 2100}; // notes durations 
//task3 
#define ONE_SECOND  1000
#define TEN 10
#define DIG_PORT PORTC
#define DIG_DDR  DDRC
#define DIG1     (1<<7) //thousands     //pin30
#define DIG2     (1<<5) //hundreds      pin 32
#define DIG3     (1<<3) //tens          pin 34
#define DIG4     (1<<1) //ones          //pin 36
#define SEG_PORT PORTA
#define SEG_DDR  DDRA
#define SEGA     (1<<6)  // aranged based on layout  pin28
#define SEGB     (1<<2)  //pin24    
#define SEGC     (1<<1)  //pin23
#define SEGD     (1<<5)  //pin27
#define SEGE     (1<<7) //pin29
#define SEGF     (1<<4) //pin26
#define SEGG     (1<<0) //pin22
#define DP       (1<<3) //pin25
// =============== Task4 ==================
#define MAX_WAIT_TIME  40
#define PRESCALE4 (1<<CS41 | 1<<CS40);
int theme_freqs[] = {293, 329, 261, 130, 196};

byte pins[] = {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG};
                                //ABCDEFG  
byte seven_seg_digits[10][7] = { { 1,1,1,1,1,1,0 },  // = 0
                                 { 0,1,1,0,0,0,0 },  // = 1
                                 { 1,1,0,1,1,0,1 },  // = 2
                                 { 1,1,1,1,0,0,1 },  // = 3
                                 { 0,1,1,0,0,1,1 },  // = 4
                                 { 1,0,1,1,0,1,1 },  // = 5
                                 { 1,0,1,1,1,1,1 },  // = 6
                                 { 1,1,1,0,0,0,0 },  // = 7
                                 { 1,1,1,1,1,1,1 },  // = 8
                                 { 1,1,1,0,0,1,1 }   // = 9
                                };


// Initializes outputs and Timer4
void setup() {
  // == Task1 ==
  LEDDDR  |= (1<<LEDBIT); //output
  LED &= ~(1<<LEDBIT); // output starts at zero
  // == Task2 ==
  SPEAKDDR |= (1<<SPEAKBIT); //output mode
  // outputs to zero
  SPEAKER = 0;
  TCCR4A = 0;
  TCCR4B = 0;
  TCNT4 = 0;
  // Set to CTC mode and prescale
  TCCR4B |= (1<<WGM42); //CTC
  TCCR4B |= PRESCALE; 
  TCCR4A |= TIMER4_CTC_A_SET;
  // === Task 3 =====
  DIG_DDR |= (DIG1|DIG2|DIG3|DIG4);
  DIG_PORT &= (~DIG1|~DIG2|~DIG3|~DIG4);
  SEG_DDR |= (SEGA|SEGB|SEGC|SEGD|SEGE|SEGF|SEGG|DP);
  SEG_PORT &= (~SEGA|~SEGB|~SEGC|~SEGD|~SEGE|~SEGF|~SEGG|~DP);
}

//check TCB.h
TCB* makeTCB(int ID, char taskName[], TCB* next) {
    TCB* curr = (TCB*) malloc(sizeof(TCB));
    if (curr == NULL) {
        Serial.print("Error: malloc was unable to allocate storage");
        return NULL;
    }
    if (curr) {
        curr->ID = ID;
        for (int i = 0; i < sizeof taskName / sizeof *taskName; i++) {
          curr->taskName[i] = taskName[i];
        }
        curr->next=next;
    }
    return curr;
}

// taskList (LinkedList) for TCB struct: DDS
TCB* task5TCB = makeTCB(5,"Supervisor", NULL);
TCB* task4TCB = makeTCB(4,"Display Music Frequency",task5TCB);
TCB* task3TCB = makeTCB(3,"Display LED", task4TCB);
TCB* task2TCB = makeTCB(2,"Play Speaker", task3TCB);
TCB* task1TCB = makeTCB(1,"Flash LED", task2TCB);
TCB* frontTCBList = task1TCB;

// List of dead tasks DDS
TCB* deadList;

//Pointer to current active task DDS
TCB* currentTask;

void loop() {
  // DDS SCHEDULER
  TCB* curr = frontTCBList;
  //front case
  while(curr->state == 4) { //Dead
    //deadList is NULL
    if (!deadList) {
      deadList = curr;
      frontTCBList = frontTCBList->next;
      deadList->next = NULL;
      curr = frontTCBList;
    } else { //deadlist not null
      frontTCBList = frontTCBList->next;
      curr->next = deadList;
      deadList = curr;
    }
    curr = frontTCBList;
  }
  // only 1 node case
  if (curr && curr->state == 1) { //Ready
    curr->state = 2;
    task_self_quit();
    currentTask = curr;
    if (curr->ID == 1) {
      task1();
    } else if (curr->ID == 2) {
      task2();
    } else if (curr->ID == 3) {
      task3p1();
    } else if (curr->ID == 4) {
      task4();
    } else if (curr->ID == 5) {
      task5();
    }
  }
  while(curr && curr->next) {
    if (curr->next->state == 1) {//Ready
      curr->next->state = 2;
      task_self_quit();
      currentTask = curr->next;
      if (curr->next->ID == 1) {
        task1();
      } else if (curr->next->ID == 2) {
        task2();
      } else if (curr->next->ID == 3) {
        task3p1();
      } else if (curr->next->ID == 4) {
        task4();
      } else if (curr->next->ID == 5) {
        task5();
      }
    } else if (curr->next->state == 2) {//Running
      
    } else if (curr->next->state == 3) { //Sleeping
      
    } else { //Dead
      //deadList is NULL
      if (!deadList) {
        deadList = curr->next;
        curr->next = curr->next->next;
        deadList->next = NULL;
      } else {
        curr->next->next = deadList;
        deadList = curr->next;
        curr->next = curr->next->next;
      }
    }
  }
}


void task_self_quit(){
  currentTask->state = 4;
}

//sets task to ready and when scheduler sees that it will start it
void task_start(TCB *task) {
  task->state = 1;
  //if the pointer to the task and front of deadList are the same
  if (deadList && deadList == task) {
    task->next = frontTCBList;
    frontTCBList = task;
    deadList = NULL;
  }
  TCB* curr = deadList;
  while(curr->next) {
    if (curr->next == task) {
      curr->next = curr->next->next;
      task->next = frontTCBList;
      frontTCBList = task;
    }
  }
}

/**
 * @briefflashes LED on for 250 ms
 *  then off for 750 ms
 */

void task1() {
  static int timer=0;
  timer++;
  if (timer==250) {
    LED &= ~(1<<LEDBIT); // LED Off
  }
  if (timer==1000) { //flash light every second for 250 ms
    LED |= (1<<LEDBIT); //LED on
    timer = 0;
  }
}

/**
 * @brief plays the tune from “Close Encounters of the Third Kind” 
 *  then does nothing for 4 seconds
 */
void task2() {
  static int songTime=0;
  static int i=0;
  static int duration = rhythm[i]; 
  if (songTime == 0) {
    SPEAKER = melody[i];
    i++;
  } 
 if (songTime == duration) {
    TCNT4 = 0;
    SPEAKER = melody[i];
    duration = duration + (rhythm[i]);
    i++;
  } 
 if (songTime > TASK_2_LENGTH) {
     TCCR4B &= (~(1 << CS42) | ~(1 << CS41) | ~(1 << CS40)); // no sound 
     duration = 0;
     songTime=0;
     i = 0;
     duration = rhythm[i];
  }
  songTime++;
  return;    
}

//===========TASK 3 ============
/*
 * @brief Increments a counter
 *  and breaks the number of the counter
 *  into its digits: thousands, hundreds, tens 
 *  and ones
 */
void task3p1() {
  static int task_3_time = 0;
  task_3_time++; 
  int numb=task_3_time/5; // temp variable to manipulate without messing up counter
  int thousands;          //       divided by 5 so it updates every 100 ms (task3p2 takes 20 ms) 
  int hundreds; 
  int tens;
  int ones;
  ones = numb % TEN;
  numb = numb / TEN;
  tens = numb % TEN;
  numb = numb / TEN;
  hundreds = numb % TEN;
  numb = numb / TEN;
  thousands = numb % TEN; 
  task3p2(thousands, hundreds, tens, ones);

  if (task_3_time ==9999) task_3_time = 9999;
  return;
}

/*
 * @brief Updates the port to output a 1 for all the true bits
 *  of the digit, and writes to the digit pin based on the digit pin
 * @params int thousands - the digit of the thousands place
 * @params int hunds - the digit of the hunds place
 * @params int tens - the digit of the tens place
 * @params int ones - the digit of the ones place
 */
void task3p2(int thousands, int hunds, int tens, int ones){
  DIG_PORT &= ~DIG1;
  DIG_PORT |= DIG4;
  DIG_PORT |= DIG3;
  DIG_PORT |= DIG2;
  for (byte segCount = 0; segCount < 7; ++segCount) {
    if (seven_seg_digits[thousands][segCount]) SEG_PORT |= (pins[segCount]); // set correct bits to true
    else SEG_PORT &= ~(pins[segCount]);
  }
  delayMicroseconds(50);
  DIG_PORT &= ~DIG2;
  DIG_PORT |= DIG4;
  DIG_PORT |= DIG3;
  DIG_PORT |= DIG1;
  for (byte segCount = 0; segCount < 7; ++segCount) {
    if (seven_seg_digits[hunds][segCount]) SEG_PORT |= (pins[segCount]); // set correct bits to true
    else SEG_PORT &= ~(pins[segCount]);
  }
  delayMicroseconds(50);
  DIG_PORT &= ~DIG3;
  DIG_PORT |= DIG4;
  DIG_PORT |= DIG2;
  DIG_PORT |= DIG1;
  for (byte segCount = 0; segCount < 7; ++segCount) {
    if (seven_seg_digits[tens][segCount]) SEG_PORT |= (pins[segCount]); // set correct bits to true
    else SEG_PORT &= ~(pins[segCount]);
  }
  delayMicroseconds(50);
  //ones place
  DIG_PORT &= ~DIG4;
  DIG_PORT |= DIG3;
  DIG_PORT |= DIG2;
  DIG_PORT |= DIG1;
  for (byte segCount = 0; segCount < 7; ++segCount) {
    if (seven_seg_digits[ones][segCount]) SEG_PORT |= (pins[segCount]); // set correct bits to true
    else SEG_PORT &= ~(pins[segCount]);
  }
  delayMicroseconds(50);
  return;
}

// =============== TASK 4 ==================
/*
 * @brief Counts down 1 digit per second from 10 
 * and increments Seg 7 display every second
 */
void task3mod() {
  //{300, 100, 400, 100, 400, 100, 400, 100, 800, 4000};  task 2 durations
  static int task_3_time = 0;
  task_3_time++; 
  int countdown=0;
  static int i = 0; // index of not being played
  int numb; // temp variable to manipulate without messing up counter
  if (task_3_time < 400) numb = theme_freqs[i];
  if (task_3_time == 400) { 
     ++i;
     numb = theme_freqs[i];
  } else if (task_3_time == 900) {
    ++i;
    numb = theme_freqs[i];
  } else if (task_3_time == 1400) {
    ++i;
    numb = theme_freqs[i];
  }else if (task_3_time == 1900) {
    ++i;
    numb = theme_freqs[i];
  } else if (task_3_time >= 2700) {
    numb = (TASK_2_LENGTH - task_3_time)/50; } //       divided by 5 so it updates every 100 ms (task3p2 takes 20 ms) 
  if (task_3_time == TASK_2_LENGTH) {
    task_3_time = 0;
    i = 0;
  }
  int thousands;         
  int hundreds; 
  int tens;
  int ones;
  ones = numb % TEN;
  numb = numb / TEN;
  tens = numb % TEN;        // if time in some range ->
  numb = numb / TEN;            //display frequency //else display numb = 40-(task3time/5)
  hundreds = numb % TEN;
  numb = numb / TEN;
  thousands = numb % TEN; 
  task3p2(thousands, hundreds, tens, ones);

}

/*
 * @brief Task 4: This task plays the theme from task 2
 *    and displays the frequency of each note while a note is being played
 *    and displays a countdown in 10 ms until the next time the theme is played
 */
void task4(){
  task2();
  task3mod();
  delayMicroseconds(1);
}

// =============== TASK 5 ==================
/*
 * @brief Continually runs task 1, runs 2 cycles of task 2, counts down for 3 seconds
 *  plays task 2 for one final time, then displays a smiley face for 2 seconds. Finally
 *  all tasks but task1 are stopped. 
 */
void task5(){
  static int task5_time=0;
  task5_time++;
  
  if (task5_time < 2 * TASK_2_LENGTH) { // two loops
    task2();
  } else if (task5_time < 2*TASK_2_LENGTH+3000) { //total time + 3 seconds
    task5p1(); // 3 second countdown
  } else if (task5_time < 3*TASK_2_LENGTH+3000) { // additional task2 time
    task2();
  } else if (task5_time < 3*TASK_2_LENGTH+5000) { // additional 2 seconds 
    task5p2(); // :)
    TCCR4B &= (~(1 << CS42) | ~(1 << CS41) | ~(1 << CS40));
  } //else { do nothing }
  task1(); //always called 
}
/*
 * @brief counts down from 3, displays one number per second 
  */
void task5p1(){
  static int task5a_time;
  task5a_time++;
  static int numb = 3;
  if (task5a_time == 1000) {
    numb--;
    task5a_time=0;}
  task3p2(0, 0, 0, numb); 
  return;
}
/*
 * @brief displays a smiley face 
  */
void task5p2() {
  DIG_PORT &= ~DIG1;
  DIG_PORT |= DIG4;
  DIG_PORT |= DIG3;
  DIG_PORT |= DIG2;
  SEG_PORT = 0; //unused in my smile
  
  delay(5);
  DIG_PORT &= ~DIG2;
  DIG_PORT |= DIG4;
  DIG_PORT |= DIG3;
  DIG_PORT |= DIG1;
  SEG_PORT |= SEGC|DP;
  
  delay(5);
  DIG_PORT &= ~DIG3;
  DIG_PORT |= DIG4;
  DIG_PORT |= DIG2;
  DIG_PORT |= DIG1;
  SEG_PORT &= ~SEGC;
  SEG_PORT |= SEGF|SEGD|SEGB|DP;
  delay(5);
  
  //ones place
  DIG_PORT &= ~DIG4;
  DIG_PORT |= DIG3;
  DIG_PORT |= DIG2;
  DIG_PORT |= DIG1;
  SEG_PORT &= ~(SEGF|SEGD|SEGB|DP);
  SEG_PORT |= SEGE;
  delay(5);
  return;
}
