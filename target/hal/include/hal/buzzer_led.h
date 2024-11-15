#ifndef BUZZER_LED_H_
#define BUZZER_LED_H_

#define RED_PERIOD "/dev/bone/pwm/1/b/period"
#define GREEN_PERIOD "/dev/bone/pwm/2/a/period"
#define BLUE_PERIOD "/dev/bone/pwm/1/a/period"

#define RED_ENABLE "/dev/bone/pwm/1/b/enable"
#define GREEN_ENABLE "/dev/bone/pwm/2/a/enable"
#define BLUE_ENABLE "/dev/bone/pwm/1/a/enable"

#define RED_CYCLE "/dev/bone/pwm/1/b/duty_cycle"
#define GREEN_CYCLE "/dev/bone/pwm/2/a/duty_cycle"
#define BLUE_CYCLE "/dev/bone/pwm/1/a/duty_cycle"

#define BUZZER_PERIOD "/dev/bone/pwm/0/a/period"
#define BUZZER_CYCLE "/dev/bone/pwm/0/a/duty_cycle"
#define BUZZER_ENABLE "/dev/bone/pwm/0/a/enable"

void Buzzer_led_init(void);
void Buzzer_led_cleanup(void);

#endif