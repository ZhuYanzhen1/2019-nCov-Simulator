#ifndef PARAMETERS_H
#define PARAMETERS_H
struct possition{
    int x;
    int y;
};
struct move_purpose{
    int direction;
    int speed;
};
struct people_para{
    possition pos;
    move_purpose move;
    bool is_admit;
    bool is_new_infect;
    unsigned long ill_time;
    unsigned char infect;
    unsigned char protect;
    unsigned char age;
};
struct virus_prob{
    float young_intensive;
    float young_critical;
    float young_died;
    float mid_intensive;
    float mid_critical;
    float mid_died;
    float old_intensive;
    float old_critical;
    float old_died;
};
struct virus_para{
    float area;
    float mild_heal;
    float intensive_heal;
    float critical_heal;
    unsigned char hide_time;
    virus_prob prob;
};
struct hospital_para{
    unsigned int vacancy;
    unsigned char diagnosis_time;
};
static const int population = 800;
static struct hospital_para hospital;
static struct virus_para virus;
static struct people_para people[population];
static int move_speed = 10;
static float protect_level = 1.0f;         //non-protect
static float flowpurpose = 0.2f;

#endif // PARAMETERS_H
