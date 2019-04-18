// ScheduleMaker.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include "ScheduleMaker.h"


using namespace std;

	Day::Day() {
		for (int i = 7; i < 24; i++) {
			times[to_string(i) + ":00"] = "";
			times[to_string(i) + ":30"] = "";

		}
	}


	Event::Event(string eName, string eDay, string eStart, string eEnd) {
		eventName = eName;
		eventDay = eDay;
		eventStart = eStart;
		eventEnd = eEnd;
	}

	Course::Course(string sj, string nm, int yr, int crsNum, vector<string> lecture, vector<string> tut, vector<string> lab) {
		subject = sj;
		name = nm;
		year = yr;
		priority = 1;
		courseNumber = crsNum;
		lectureTime = lecture;
		tutorialTime = tut;
		labTime = lab;
	}
	Courses::Courses() {
		vector<string> courseTimes;
		vector<string> tutorialTime;
		vector<string> labTime;
		string courseName;
		string courseCode;
		string courseTime;
		string courseNumber;
		ifstream coursesText("Courses.txt");
		if (coursesText.is_open())
			while (!coursesText.eof()) {
				coursesText >> courseCode;
				coursesText >> courseNumber;
				cout << courseCode << endl;
				coursesText >> courseName;
				cout << courseName << endl;
				coursesText >> courseTime;
				while (courseTime != "Tutorial") {
					courseTimes.push_back(courseTime); // add all times to a vector, index zero to section 1, etc...
					coursesText >> courseTime;
				}
				cout << courseTime;
				coursesText >> courseTime;
				cout << courseTime;
				while (courseTime != "Lab") {
					tutorialTime.push_back(courseTime);
					coursesText >> courseTime;
				}
				cout << courseTime;
				coursesText >> courseTime;
				while (courseTime != ("\\")) {
					labTime.push_back(courseTime);
					coursesText >> courseTime;
				}
				Course newCourse(courseCode, courseName, 2, stoi(courseNumber), courseTimes, tutorialTime, labTime);
				this->courseClassMap.insert(make_pair(stoi(courseNumber), newCourse));
			}
		else
			cout << "Courses test doesn't open!" << endl;
	}
	//hash based on the course number

	int Course::getCourseNumber() {
		return courseNumber;
	}

	Course Courses::searchSubj(const int key, unordered_map<int, Course> mymap) {
		auto itInner = mymap.find(key);
		Course search = itInner->second;
		return search;
		// overload is the problem, lets take a look operator overloading before and try and add that bad boy in to compare the courses
	}


	Schedule::Schedule() {
		for (int i = 0; i < 7; i++) {
			schedule.push_back(Day());
		}
	}

	Schedule::Schedule(const Schedule& right) {
		schedule = right.schedule;
	}




	bool ScheduleWrapper::add_course(const Course& newCourse) {
		if (enrolled.size() == 8) {
			return false;
		}
		for (int i = 0; i < enrolled.size(); i++) {
			if (enrolled[i] == newCourse) {
				return false;
			}
		}
		enrolled.push_back(newCourse);

		return true;
	}

	void ScheduleWrapper::remove_course(const Course &removedCourse) {
		enrolled.erase(std::remove(enrolled.begin(), enrolled.end(), removedCourse));
	}

	bool ScheduleWrapper::check_placing(Schedule s, string d, vector<string> time_slots) {
		int day;
		if (d == "Monday") { day = 0; }
		else if (d == "Tuesday") { day = 1; }
		else if (d == "Wednesday") { day = 2; }
		else if (d == "Thursday") { day = 3; }
		else if (d == "Friday") { day = 4; }
		else if (d == "Saturday") { day = 5; }
		else { day = 6; }

		for (int i = 0; i < time_slots.size(); i++) {
			if (s.schedule[day].times[time_slots[i]] != "") {
				course_conflicts.push_back(s.schedule[day].times[time_slots[i]]);
				return false;
			}
		}
		return true;
	}

	bool ScheduleWrapper::can_place_lec(Course c, Schedule s, int time_slot) {
		string str = c.lectureTime[time_slot];
		vector<string> c_times = parse_times(str);

		for (int i = 0; i < c_times.size(); i = i + 3) {
			vector<string> time_slots = get_time_slots(c_times[i + 1], c_times[i + 2]);
			bool is_placeable = check_placing(s, c_times[i], time_slots);
			if (!is_placeable) {
				course_conflicts.push_back(c.name);
				return false;
			}
		}
		return true;
	}

	void ScheduleWrapper::place_course(Course c, Schedule& s, int time_slot) {
		string str = c.lectureTime[time_slot];
		vector<string> c_times = parse_times(str);

		for (int i = 0; i < c_times.size(); i = i + 3) {
			vector<string> time_slots = get_time_slots(c_times[i + 1], c_times[i + 2]);
			int day;
			if (c_times[i] == "Monday") { day = 0; }
			else if (c_times[i] == "Tuesday") { day = 1; }
			else if (c_times[i] == "Wednesday") { day = 2; }
			else if (c_times[i] == "Thursday") { day = 3; }
			else if (c_times[i] == "Friday") { day = 4; }
			else if (c_times[i] == "Saturday") { day = 5; }
			else { day = 6; }
			for (int j = 0; j < time_slots.size(); j++) {
				s.schedule[day].times[time_slots[j]] = c.name + " Lecture";
			}
		}
	}

	bool ScheduleWrapper::can_place_tut(Course c, Schedule& s, int time_slot) {
		string str = c.tutorialTime[time_slot];
		vector<string> c_times = parse_times(str);

		for (int i = 0; i < c_times.size(); i = i + 3) {
			vector<string> time_slots = get_time_slots(c_times[i + 1], c_times[i + 2]);
			bool is_placeable = check_placing(s, c_times[i], time_slots);
			if (!is_placeable) {
				course_conflicts.push_back(c.name);
				return false;
			}
		}

		return true;
	}

	void ScheduleWrapper::place_tut(Course c, Schedule& s, int time_slot) {
		string str = c.tutorialTime[time_slot];
		vector<string> c_times = parse_times(str);

		for (int i = 0; i < c_times.size(); i = i + 3) {
			vector<string> time_slots = get_time_slots(c_times[i + 1], c_times[i + 2]);
			int day;
			if (c_times[i] == "Monday") { day = 0; }
			else if (c_times[i] == "Tuesday") { day = 1; }
			else if (c_times[i] == "Wednesday") { day = 2; }
			else if (c_times[i] == "Thursday") { day = 3; }
			else if (c_times[i] == "Friday") { day = 4; }
			else if (c_times[i] == "Saturday") { day = 5; }
			else { day = 6; }
			for (int j = 0; j < time_slots.size(); j++) {
				s.schedule[day].times[time_slots[j]] = c.name + " Tutorial";
			}
		}
	}

	bool ScheduleWrapper::can_place_lab(Course c, Schedule& s, int time_slot) {
		string str = c.labTime[time_slot];
		vector<string> c_times = parse_times(str);

		for (int i = 0; i < c_times.size(); i = i + 3) {
			vector<string> time_slots = get_time_slots(c_times[i + 1], c_times[i + 2]);
			bool is_placeable = check_placing(s, c_times[i], time_slots);
			if (!is_placeable) {
				course_conflicts.push_back(c.name);
				return false;
			}
		}
		return true;
	}

	void ScheduleWrapper::place_lab(Course c, Schedule& s, int time_slot) {
		string str = c.labTime[time_slot];
		vector<string> c_times = parse_times(str);

		for (int i = 0; i < c_times.size(); i = i + 3) {
			vector<string> time_slots = get_time_slots(c_times[i + 1], c_times[i + 2]);
			int day;
			if (c_times[i] == "Monday") { day = 0; }
			else if (c_times[i] == "Tuesday") { day = 1; }
			else if (c_times[i] == "Wednesday") { day = 2; }
			else if (c_times[i] == "Thursday") { day = 3; }
			else if (c_times[i] == "Friday") { day = 4; }
			else if (c_times[i] == "Saturday") { day = 5; }
			else { day = 6; }
			for (int j = 0; j < time_slots.size(); j++) {
				s.schedule[day].times[time_slots[j]] = c.name + " Lab";
			}
		}
	}

	bool ScheduleWrapper::can_place_event(Event e, Schedule& s) {
		vector<string> time_slots = get_time_slots(e.eventStart, e.eventEnd);
		bool is_placeable = check_placing(s, e.eventDay, time_slots);
		if (!is_placeable) {
			event_conflicts.push_back(e.eventName);
			return false;
		}
		return true;
	}

	void ScheduleWrapper::place_event(Event e, Schedule& s) {
		string eStart = e.eventStart;
		string eEnd = e.eventEnd;
		vector<string> time_slots = get_time_slots(eStart, eEnd);
		int day;
		if (e.eventDay == "Monday") { day = 0; }
		else if (e.eventDay == "Tuesday") { day = 1; }
		else if (e.eventDay == "Wednesday") { day = 2; }
		else if (e.eventDay == "Thursday") { day = 3; }
		else if (e.eventDay == "Friday") { day = 4; }
		else if (e.eventDay == "Saturday") { day = 5; }
		else { day = 6; }
		for (int j = 0; j < time_slots.size(); j++) {
			s.schedule[day].times[time_slots[j]] = e.eventName;
		}
	}

	void ScheduleWrapper::schedule_events() {
		for (int i = 0; i < result.size(); i++) {
			bool added_all_events = true;
			for (int j = 0; j < events.size(); j++) {
				if (!can_place_event(events[j], result[i])) {
					added_all_events = false;
				}
			}
			if (added_all_events) {
				Schedule new_schedule = result[i];
				for (int j = 0; j < events.size(); j++) {
					place_event(events[j], new_schedule);
				}
				schedule_with_events.push_back(new_schedule);
			}
		}
	}

	void ScheduleWrapper::optimize_morning() {
		Schedule new_schedule = Schedule();
		helper_morning(0, false, false, false, new_schedule);
	}

	void ScheduleWrapper::helper_morning(int index, bool lecturePlaced, bool tutorialPlaced, bool labPlaced, Schedule curr) {
		if (index >= enrolled.size()) {
			result.push_back(curr);
		}
		else {

			if (!lecturePlaced) {
				for (int i = 0; i < enrolled[index].lectureTime.size(); i++) {
					if (can_place_lec(enrolled[index], curr, i)) {
						Schedule new_schedule = curr;
						place_course(enrolled[index], new_schedule, i);
						helper_morning(index, true, false, false, new_schedule);
					}
				}
			}
			else if (!tutorialPlaced) {
				if (enrolled[index].tutorialTime.size() != 0) {
					for (int i = 0; i < enrolled[index].tutorialTime.size(); i++) {
						if (can_place_tut(enrolled[index], curr, i)) {
							Schedule new_schedule = curr;
							place_tut(enrolled[index], new_schedule, i);
							helper_morning(index, true, true, false, new_schedule);
						}
					}
				}
				else {
					Schedule new_schedule = curr;
					helper_morning(index, true, true, false, new_schedule);
				}
			}
			else {
				if (enrolled[index].labTime.size() != 0) {
					for (int i = 0; i < enrolled[index].labTime.size(); i++) {
						if (can_place_lab(enrolled[index], curr, i)) {
							Schedule new_schedule = curr;
							place_lab(enrolled[index], new_schedule, i);
							int new_index = index + 1;
							helper_morning(new_index, false, false, false, new_schedule);
						}
					}
				}
				else {
					Schedule new_schedule = curr;
					int new_index = index + 1;
					helper_morning(new_index, false, false, false, new_schedule);
				}
			}
		}
	}

	void ScheduleWrapper::optimize_evening() {
		Schedule new_schedule = Schedule();
		helper_morning(0, false, false, false, new_schedule);
	}

	void ScheduleWrapper::helper_evening(int index, bool lecturePlaced, bool tutorialPlaced, bool labPlaced, Schedule curr) {
		if (index >= enrolled.size()) {
			result.push_back(curr);
		}
		else {

			if (!lecturePlaced) {
				for (int i = enrolled[index].lectureTime.size() - 1; i > -1; i--) {
					if (can_place_lec(enrolled[index], curr, i)) {
						Schedule new_schedule = curr;
						place_course(enrolled[index], new_schedule, i);
						helper_evening(index, true, false, false, new_schedule);
					}
				}
			}
			else if (!tutorialPlaced) {
				if (enrolled[index].tutorialTime.size() != 0) {
					for (int i = enrolled[index].tutorialTime.size() - 1; i > -1; i--) {
						if (can_place_tut(enrolled[index], curr, i)) {
							Schedule new_schedule = curr;
							place_tut(enrolled[index], new_schedule, i);
							helper_evening(index, true, true, false, new_schedule);
						}
					}
				}
				else {
					Schedule new_schedule = curr;
					helper_evening(index, true, true, false, new_schedule);
				}
			}
			else {
				if (enrolled[index].labTime.size() != 0) {
					for (int i = enrolled[index].labTime.size() - 1; i > -1; i--) {
						if (can_place_lab(enrolled[index], curr, i)) {
							Schedule new_schedule = curr;
							place_lab(enrolled[index], new_schedule, i);
							int new_index = index + 1;
							helper_evening(new_index, false, false, false, new_schedule);
						}
					}
				}
				else {
					Schedule new_schedule = curr;
					int new_index = index + 1;
					helper_evening(new_index, false, false, false, new_schedule);
				}
			}
		}
	}


	vector<string> ScheduleWrapper::split(string str, string sep) {
		char* cstr = const_cast<char*>(str.c_str());
		char* current;
		vector<std::string> arr;
		current = strtok(cstr, sep.c_str());
		while (current != NULL) {
			arr.push_back(current);
			current = strtok(NULL, sep.c_str());
		}
		return arr;
	}

	vector<string> ScheduleWrapper::get_time_slots(string start_time, string end_time) {
		int start;
		if (start_time.size() > 3) {
			start = atoi(start_time.substr(0, 2).c_str());
		}
		else {
			start = atoi(start_time.substr(0, 1).c_str());
		}
		int end;
		if (end_time.size() > 3) {
			end = atoi(end_time.substr(0, 2).c_str());
		}
		else {
			end = atoi(end_time.substr(0, 1).c_str());
		}

		vector<string> time_slots;
		for (int i = start; i < end + 1; i++) {
			if (to_string(i) + ":00" == end_time) {
				break;
			}
			if (i != start) {
				time_slots.push_back(to_string(i) + ":00");
			}
			if (to_string(i) + ":30" == end_time) {
				break;
			}
			time_slots.push_back(to_string(i) + ":30");
		}

		return time_slots;
	}

	bool ScheduleWrapper::add_event(Event newEvent) {
		if (events.size() == 5) {
			return false;
		}
		events.push_back(newEvent);

		return true;
	}

	void ScheduleWrapper::remove_event(Event removedEvent) {
		events.erase(std::remove(events.begin(), events.end(), removedEvent));
	}

	vector<string> ScheduleWrapper::parse_times(string times) {
		vector<string> days;
		days = split(times, " ");
		vector<string> day_times;
		vector<string> split_times;

		for (int i = 0; i < days.size(); i++) {
			vector<string> temp = split(days[i], ",");
			for (int j = 0; j < temp.size(); j++) {
				day_times.push_back(temp[j]);
			}
		}
		for (int i = 0; i < day_times.size(); i++) {
			vector<string> temp = split(day_times[i], "-");
			for (int j = 0; j < temp.size(); j++) {
				split_times.push_back(temp[j]);
			}
		}
		for (int i = 0; i < split_times.size(); i++) {
			split_times[i];
		}
		return split_times;
	}

	void ScheduleWrapper::showfirst() {
		if (schedule_with_events.size() == 0) {
			if (event_conflicts.size() > 0) {
				cout << event_conflicts[0] << endl;
			}
		}
		else {
			for (int j = 0; j < 7; j++) {
				for (int i = 7; i < 24; i++) {
					if (schedule_with_events[0].schedule[j].times[to_string(i) + ":00"] != "") {
						cout << j << " " + to_string(i) + ":00 " << schedule_with_events[0].schedule[j].times[to_string(i) + ":00"] << endl;
					}
					if (schedule_with_events[0].schedule[j].times[to_string(i) + ":30"] != "") {
						cout << j << " " + to_string(i) + ":30 " << schedule_with_events[0].schedule[j].times[to_string(i) + ":30"] << endl;
					}
				}
			}
		}
	}
