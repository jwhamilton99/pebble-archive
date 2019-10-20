#pragma once

void update_timer_text();

int minutes;
int seconds;

int mem_minutes;
int mem_seconds;

void reset_elements();
void stop_actionbar_timeout();

void show_action_bar();

void restart_timer();

int elapsed_seconds;
int elapsed_minutes;