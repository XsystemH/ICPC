#include <iostream>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>
#include <set>
#include <unordered_map>
#include <queue>
#include <algorithm>

int problem;
int duration_time;
bool froze = false;
bool started = false;
int teamnum = 0;

struct CompareGreater 
{
  bool operator()(int a, int b) const 
  {
    return a > b; // from big to small
  }
};

struct team
{
  std::string name;
  int acnum = 0;
  int time = 0;
  int ac[26] = {0}; // the time used to ac
  bool didac[26] = {false};
  bool altready[26] = {false};
  int st[26] = {0}; // the times submitted befor e ac
  int fst[26] = {0}; // the times submitted when froze
  int last_submit[26] = {0}; // query last submit in the given problem 1 ac 2 wa 3 re 4 tle
  int last_stat[26][4] = {-1}; // store time 1 ac 2 wa 3 re 4 tle
  int last_time[26] = {0}; // query last submit in the given problem
  int last_prob = -1;
  std::multiset<int, CompareGreater> act;
  // int last_ac = -1;
  // int last_wa = -1;
  // int last_re = -1;
  // int last_tle = -1; // -1 means no such submission
  // it may be covered, need to remember time
  std::pair<int, int> last_ac = std::make_pair(-1, -1);
  std::pair<int, int> last_wa = std::make_pair(-1, -1);
  std::pair<int, int> last_re = std::make_pair(-1, -1);
  std::pair<int, int> last_tle = std::make_pair(-1, -1);
  int ac_in_froze[26] = {0};
  
  friend bool operator<(const team& a, const team& b)
  {
    if (a.acnum != b.acnum) return a.acnum > b.acnum;
    else if (a.time != b.time) return a.time < b.time;
    else
    {
      for (auto iter1 = a.act.begin(), iter2 = b.act.begin(); iter1 != a.act.end() && iter2 != b.act.end(); iter1++, iter2++)
      {
        if (*iter1 != *iter2) return *iter1 < *iter2;
      }
      return a.name < b.name;
    }
  }
};

std::unordered_map<std::string, team> teams; // fixed storage
std::unordered_map<std::string, int> list; // query

struct quick
{
  std::string name;
  int acnum = 0;
  int time = 0;
  std::multiset<int, CompareGreater> act;

  friend bool operator<(const quick& a, const quick& b)
  {
    if (a.acnum != b.acnum) return a.acnum < b.acnum;
    else if (a.time != b.time) return a.time > b.time;
    else
    {
      for (auto iter1 = a.act.begin(), iter2 = b.act.begin(); iter1 != a.act.end() && iter2 != b.act.end(); iter1++, iter2++)
      {
        if (*iter1 != *iter2) return *iter1 > *iter2;
      }
      return a.name > b.name;
    }
  }
};
std::set<quick> rank;

void Addteam()
{
  std::string name;
  std::cin >> name;
  if (started == true)
  {
    std::cout<<"[Error]Add failed: competition has started.\n";
    return;
  }
  else if (teams.find(name) != teams.end()) // O(logN)
  {
    std::cout<<"[Error]Add failed: duplicated team name.\n";
    return;
  }
  std::cout<<"[Info]Add successfully.\n";
  team newteam;
  newteam.name = name;
  teams[name]=newteam; // O(1)
  quick newquick;
  newquick.name = name;
  rank.insert(newquick); // O(logN)
  // temporarily, fresh when start
  teamnum++;
  return;
}

void Start()
{
  if (started)
  {
    std::cout << "[Error]Start failed: competition has started.\n";
    return;
  }
  started = true;
  std::string temp;
  std::cin >> temp; // pass "DURATION"
  std::cin >> duration_time;
  std::cin >> temp;
  std::cin >> problem;
  int t = teamnum - 1;
  std::cout << "[Info]Competition starts.\n";
  for (auto iter = rank.begin(); iter != rank.end(); iter++)
  {
    list[iter->name] = t;
    t--;
  }
  return;
}

void Submit()
{
  char problem_name;
  std::string name;
  std::string temp;
  std::string submit_status;
  int time;
  std::cin >> problem_name >> temp >> name >> temp >> submit_status >> temp >> time;
  if(time < 1 || time > duration_time)
  {
    return;
  }
  if (submit_status == "Accepted")
  {
    teams[name].last_submit[int(problem_name - 'A')] = 1;
    if (froze)
    {
      if (!teams[name].altready[int(problem_name - 'A')]) teams[name].fst[int(problem_name - 'A')]++;
      if (!teams[name].didac[int(problem_name - 'A')]) // if  there is unac submission befor e
      {
        teams[name].ac[int(problem_name - 'A')] = time;
        teams[name].ac_in_froze[int(problem_name - 'A')] = teams[name].fst[int(problem_name - 'A')];
      }
      teams[name].last_ac = std::make_pair(problem_name - 'A', time);
      teams[name].last_prob = problem_name - 'A';
      teams[name].last_time[problem_name - 'A'] = time;
      teams[name].didac[int(problem_name - 'A')] = true;
      teams[name].last_stat[int(problem_name - 'A')][0] = time;
      return;
    }
    else 
    {
      quick subteam;
      subteam.name = name;
      subteam.acnum = teams[name].acnum;
      subteam.time = teams[name].time;
      subteam.act = teams[name].act;
      rank.erase(subteam);
      if (!teams[name].didac[int(problem_name - 'A')])
      {
        teams[name].acnum++;
        teams[name].st[int(problem_name - 'A')]++;
        teams[name].act.insert(time);
        teams[name].ac[int(problem_name - 'A')] = time;
        teams[name].time += 20*(teams[name].st[int(problem_name - 'A')] - 1) + teams[name].ac[int(problem_name - 'A')]; // punish time
      }
      teams[name].last_ac = std::make_pair(problem_name - 'A', time);
      teams[name].last_prob = problem_name - 'A';
      teams[name].last_time[problem_name - 'A'] = time;
      teams[name].didac[int(problem_name - 'A')] = true;
      teams[name].altready[int(problem_name - 'A')] = true;
      teams[name].last_stat[int(problem_name - 'A')][0] = time;
      subteam.acnum = teams[name].acnum;
      subteam.time = teams[name].time;
      subteam.act = teams[name].act;
      rank.insert(subteam);
      return;
    }
  }
  else
  {
    if (froze)
    {
      if (!teams[name].altready[int(problem_name - 'A')]) teams[name].fst[int(problem_name - 'A')]++;
      if (submit_status == "Wrong_Answer")
      {
        teams[name].last_submit[int(problem_name - 'A')] = 2;
        teams[name].last_wa = std::make_pair(problem_name - 'A', time);
        teams[name].last_stat[int(problem_name - 'A')][1] = time;
      }
      if (submit_status == "Runtime_Error")
      {
        teams[name].last_submit[int(problem_name - 'A')] = 3;
        teams[name].last_re = std::make_pair(problem_name - 'A', time);
        teams[name].last_stat[int(problem_name - 'A')][2] = time;
      }
      if (submit_status == "Time_Limit_Exceed")
      {
        teams[name].last_submit[int(problem_name - 'A')] = 4;
        teams[name].last_tle = std::make_pair(problem_name - 'A', time);
        teams[name].last_stat[int(problem_name - 'A')][3] = time;
      }
      teams[name].last_prob = problem_name - 'A';
      teams[name].last_time[problem_name - 'A'] = time;
    }
    else
    {
      quick subteam;
      subteam.name = name;
      subteam.acnum = teams[name].acnum;
      subteam.time = teams[name].time;
      subteam.act = teams[name].act;
      rank.erase(subteam);
      if (!teams[name].didac[int(problem_name - 'A')])
      {
        teams[name].st[int(problem_name - 'A')]++;
      }
      if (submit_status == "Wrong_Answer")
      {
        teams[name].last_submit[int(problem_name - 'A')] = 2;
        teams[name].last_wa = std::make_pair(problem_name - 'A', time);
        teams[name].last_stat[int(problem_name - 'A')][1] = time;
      }
      if (submit_status == "Runtime_Error")
      {
        teams[name].last_submit[int(problem_name - 'A')] = 3;
        teams[name].last_re = std::make_pair(problem_name - 'A', time);
        teams[name].last_stat[int(problem_name - 'A')][2] = time;
      }
      if (submit_status == "Time_Limit_Exceed")
      {
        teams[name].last_submit[int(problem_name - 'A')] = 4;
        teams[name].last_tle = std::make_pair(problem_name - 'A', time);
        teams[name].last_stat[int(problem_name - 'A')][3] = time;
      }
      teams[name].last_prob = problem_name - 'A';
      teams[name].last_time[problem_name - 'A'] = time;
      rank.insert(subteam);
    }
  }
}

void Flush()
{
  int t = teamnum - 1;
  list.clear();
  for (auto iter = rank.begin(); iter != rank.end(); iter++)
  {
    list[iter->name] = t;
    t--;
  }
  std::cout<<"[Info]Flush scoreboard.\n";
  return;
}

void Freeze()
{
  if (froze) std::cout << "[Error]Freeze failed: scoreboard has been frozen.\n";
  else std::cout << "[Info]Freeze scoreboard.\n";
  froze = true;
  return;
}

void PrintList()
{
  int r = 1;
  for (auto iter = rank.rbegin(); iter != rank.rend(); iter++)
  {
    std::cout << iter->name << " " << r << " " << iter->acnum << " " << iter->time;
    for (int i = 0; i < problem; i++) // 空格承前
    {
      if (teams[iter->name].didac[i] && teams[iter->name].fst[i] == 0)
      {
        std::cout << " +";
        if(teams[iter->name].st[i] > 1) std::cout << teams[iter->name].st[i] -1;
      }
      else if(!teams[iter->name].didac[i] && teams[iter->name].fst[i] == 0)
      {
        if(teams[iter->name].st[i] != 0)
        {
          std::cout << " -";
          std::cout << teams[iter->name].st[i];
        }
        else std::cout << " .";
      }
      if (teams[iter->name].fst[i] > 0)
      {
        if (teams[iter->name].st[i] != 0) std::cout << " -";
        else std::cout << " ";
        std::cout << teams[iter->name].st[i] << "/" << teams[iter->name].fst[i];
      }
    }
    r++;
    std::cout << "\n";
  }
}

void Scroll()
{
  if (!froze)
  {
    std::cout << "[Error]Scroll failed: scoreboard has not been frozen.\n";
    return;
  }
  std::cout << "[Info]Scroll scoreboard.\n";
  int r = 1;
  PrintList();
  std::string cr, next;
  bool flag = false;
  auto iter = rank.begin();
  while (iter != rank.end())
  {
    cr = iter->name;
    flag = false;
    for (int i = 0; i < problem; i++)
    {
      if (teams[cr].fst[i] != 0 && teams[cr].ac_in_froze[i] == 0 && !teams[cr].altready[i])
      {
        teams[cr].st[i] += teams[cr].fst[i];
        teams[cr].fst[i] = 0;
      }
      if (teams[cr].fst[i] != 0 && teams[cr].ac_in_froze[i] != 0)
      {
        quick subteam = *iter;
        iter++;
        quick nextteam = *iter;
        flag = true;
        rank.erase(subteam);
        teams[cr].st[i] += teams[cr].ac_in_froze[i];
        teams[cr].fst[i] = 0;
        teams[cr].ac_in_froze[i] = 0;
        teams[cr].altready[i] = true;
        teams[cr].acnum++;
        teams[cr].time += 20*(teams[cr].st[i] - 1) + teams[cr].ac[i]; // punish time
        teams[cr].act.insert(teams[cr].ac[i]);
        subteam.acnum = teams[cr].acnum;
        subteam.time = teams[cr].time;
        subteam.act = teams[cr].act;
        rank.insert(subteam);
        if (nextteam < subteam && iter != rank.end())
        {
          iter = rank.find(nextteam);
          auto it = rank.find(subteam);
          it--;
          std::cout << cr << " " << it->name << " " << teams[cr].acnum << " " << teams[cr].time << "\n";
        }
        else iter = rank.find(subteam);
        break;
      }
    }
    if (!flag) iter++;
  }
  PrintList();
  froze = false;
  int t = teamnum - 1;
  list.clear();
  for (auto iter1 = rank.begin(); iter1 != rank.end(); iter1++)
  {
    list[iter1->name] = t;
    t--;
  }
  return;
}

void QueryName()
{
  std::string name;
  std::cin >> name;
  if (list.find(name) == list.end())
  {
    std::cout << "[Error]Query ranking failed: cannot find the team.\n";
    return;
  }
  std::cout << "[Info]Complete query ranking.\n";
  if (froze) std::cout << "[Warning]Scoreboard is frozen. The ranking may be inaccurate until it were scrolled.\n";
  std::cout << name <<" NOW AT RANKING " << list[name] + 1 << "\n";
  return;
}

void QuerySubmit()
{
  std::string name;
  std::string temp;
  std::string prob;
  int p;
  std::string stat;
  std::cin >> name >> temp >> prob >> temp >> stat;
  p = prob.back() - 'A'; // 

  if (list.find(name) == list.end())
  {
    std::cout << "[Error]Query submission failed: cannot find the team.\n";
    return;
  }
  std::cout << "[Info]Complete query submission.\n";

  if (prob == "PROBLEM=ALL" && stat == "STATUS=ALL") // all all
  {
    if (teams[name].last_prob == -1)
    {
      std::cout << "Cannot find any submission.\n";
      return;
    }
    std::cout << name << " " << char('A' + teams[name].last_prob);
    if (teams[name].last_submit[teams[name].last_prob] == 1) std::cout << " Accepted "; // << teams[name].last_submit[teams[name].last_prob];
    if (teams[name].last_submit[teams[name].last_prob] == 2) std::cout << " Wrong_Answer ";
    if (teams[name].last_submit[teams[name].last_prob] == 3) std::cout << " Runtime_Error ";
    if (teams[name].last_submit[teams[name].last_prob] == 4) std::cout << " Time_Limit_Exceed ";
    std::cout << teams[name].last_time[teams[name].last_prob] << "\n";
    return;
  }

  // last stat
  else if (prob == "PROBLEM=ALL" && stat == "STATUS=Accepted")
  {
    if (teams[name].last_ac.first == -1)
    {
      std::cout << "Cannot find any submission.\n";
      return;
    }
    std::cout << name << " " << char('A' + teams[name].last_ac.first) << " Accepted " << teams[name].last_ac.second << "\n";
    return;
  }
  else if (prob == "PROBLEM=ALL" && stat == "STATUS=Wrong_Answer")
  {
    if (teams[name].last_wa.first == -1)
    {
      std::cout << "Cannot find any submission.\n";
      return;
    }
    std::cout << name << " " << char('A' + teams[name].last_wa.first) << " Wrong_Answer " << teams[name].last_wa.second << "\n";
    return;
  }
  else if (prob == "PROBLEM=ALL" && stat == "STATUS=Runtime_Error")
  {
    if (teams[name].last_re.first == -1)
    {
      std::cout << "Cannot find any submission.\n";
      return;
    }
    std::cout << name << " " << char('A' + teams[name].last_re.first) << " Runtime_Error " << teams[name].last_re.second << "\n";
    return;
  }
  else if (prob == "PROBLEM=ALL" && stat == "STATUS=Time_Limit_Exceed")
  {
    if (teams[name].last_tle.first == -1)
    {
      std::cout << "Cannot find any submission.\n";
      return;
    }
    std::cout << name << " " << char('A' + teams[name].last_tle.first) << " Time_Limit_Exceed " << teams[name].last_tle.second << "\n";
    return;
  }

  // specif ic prob's last
  else if (stat == "STATUS=ALL")
  {
    if (teams[name].last_submit[p] == 0)
    {
      std::cout << "Cannot find any submission.\n";
      return;
    }
    std::cout << name << " " << prob.back();
    if (teams[name].last_submit[p] == 1) std::cout << " Accepted "; // << teams[name].last_submit[teams[name].last_prob];
    if (teams[name].last_submit[p] == 2) std::cout << " Wrong_Answer ";
    if (teams[name].last_submit[p] == 3) std::cout << " Runtime_Error ";
    if (teams[name].last_submit[p] == 4) std::cout << " Time_Limit_Exceed ";
    std::cout << teams[name].last_stat[p][teams[name].last_submit[p] - 1] << "\n";
    return;
  }
  else if (stat == "STATUS=Accepted")
  {
    if (teams[name].last_stat[p][0] < 1)
    {
      std::cout << "Cannot find any submission.\n";
      return;
    }
    std::cout << name << " " << char('A' + p) << " Accepted " << teams[name].last_stat[p][0] << "\n";
    return;
  }
  else if (stat == "STATUS=Wrong_Answer")
  {
    if (teams[name].last_stat[p][1] < 1)
    {
      std::cout << "Cannot find any submission.\n";
      return;
    }
    std::cout << name << " " << char('A' + p) << " Wrong_Answer " << teams[name].last_stat[p][1] << "\n";
    return;
  }
  else if (stat == "STATUS=Runtime_Error")
  {
    if (teams[name].last_stat[p][2] < 1)
    {
      std::cout << "Cannot find any submission.\n";
      return;
    }
    std::cout << name << " " << char('A' + p) << " Runtime_Error " << teams[name].last_stat[p][2] << "\n";
    return;
  }
  else if (stat == "STATUS=Time_Limit_Exceed")
  {
    if (teams[name].last_stat[p][3] < 1)
    {
      std::cout << "Cannot find any submission.\n";
      return;
    }
    std::cout << name << " " << char('A' + p) << " Time_Limit_Exceed " << teams[name].last_stat[p][3] << "\n";
    return;
  }
  return;
}

int main()
{
  std::ios_base::sync_with_stdio(false);
  std::setbuf(stdout, NULL);
  freopen("1.in","r",stdin);
  freopen("1.txt","w",stdout);
  std::string temp;
  while(std::cin>>temp)
  {
    if (temp == "ADDTEAM") Addteam();
    else if (temp == "START") Start();
    else if (temp == "SUBMIT") Submit();
    else if (temp == "FLUSH") Flush();
    else if (temp == "FREEZE") Freeze();
    else if (temp == "SCROLL") Scroll();
    else if (temp == "QUERY_RANKING") QueryName();
    else if (temp == "QUERY_SUBMISSION") QuerySubmit();
    else if (temp == "END") break;
  }
  std::cout << "[Info]Competition ends.\n";
  return 0;
}