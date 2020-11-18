#ifndef _BUZZER_H_
#define _BUUZER_H_


void doHelp(); // 도움말 출력하는 함수
int buzzerInit(); // Buzzer 설정 On
int buzzerPlaySound(int scale); // Buzzer 사운드 출력 함수(scale은 음(도레미...)) 
int buzzerStopSound(); // Buzzer 사운드 그만하는 함수
void WarningRED();
void WarningYELLOW();
int buzzerExit(); // Buzzer 설정 Off

#endif
