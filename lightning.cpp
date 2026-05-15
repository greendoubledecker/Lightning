#include <bits/stdc++.h>
#include <windows.h>
#include <cstdlib>
using namespace std;
using ld = long double;
using ll = long long;
vector<ll> ground;
vector<vector<ll>> air;
vector<vector<bool>> visited;
ld multiplier = 5;
ll tgtx = -1, tgty = 100000;

void preciseDelay(chrono::milliseconds duration){
	auto start = chrono::steady_clock::now();
	while(chrono::steady_clock::now() - start < duration){}
}

void lightningAt(ll row, ll col){
	++col;
	cout << "\033[" << row << ";" << col << "H" << "\033[48;2;160;190;10m \033[0m" << flush;
}
void brightLightning(ll row, ll col){
	++col;
	cout << "\033[" << row << ";" << col << "H" << "\033[48;2;255;255;160m \033[0m" << flush;
}

struct CustomCompare{
	bool operator()(const pair<ll, ll>& a, const pair<ll, ll>& b){
		return (ld)air[a.first][a.second] + multiplier * hypot(a.first - tgtx, a.second - tgty) > (ld)air[b.first][b.second] + multiplier * hypot(b.first - tgtx, b.second - tgty);
	}
};

int main(){
	//----------------------------------------SETTING UP GROUND AND AIR----------------------------------------

	system("cls");
	srand(time(0));
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	ll cols, rows;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if(GetConsoleScreenBufferInfo(hConsole, &csbi)){
		cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	}else{
		cout << "Failed to retrieve terminal size. Code: " << GetLastError() << '\n';
		return 0;
	}
	air = vector<vector<ll>>(cols, vector<ll>(rows));
	visited = vector<vector<bool>>(cols, vector<bool>(rows));
	for(ll i = 0; i < cols; ++i){
		for(ll j = 0; j < rows; ++j){
			air[i][j] = rand() % 256;
		}
	}
	cout << "\033[38;5;94m";
	ground.push_back(rows - (rand() % (rows/10) + rows/10));
	for(ll i = 0; i < cols - 1; ++i){
		ll step = (rand() % 7) - 3;
		step *= 2;
		step /= 5;
		ground.push_back(min(ground[i] + step, rows - 2));
	}
	bool side = rand() % 2; //false means target closer to left, true means closer to right
	for(ll i = 0; i < cols; ++i){
		if(side && ground[i] == tgty){
			tgtx = i;
		}
		if(ground[i] < tgty){
			tgtx = i;
			tgty = ground[i];
		}
	}
	tgty -= 2;
	for(ll i = 0; i < rows; ++i){
		for(ll j = 0; j < cols; ++j){
			if((i == tgty || i == tgty + 1) && j == tgtx){
				cout << "\033[48;2;70;60;255;38;2;240;20;20m$\033[0m"; //lightning rod!
				continue;
			}
			if(ground[j] > i){
				cout << "\033[48;2;70;" << air[j][i] << ";255m \033[0m"; //air
			}else if(ground[j] < i){
				cout << "\033[48;5;94m \033[0m"; //ground
			}else{
				cout << "\033[48;2;70;60;255;38;2;0;255;140m@\033[0m"; //grass
			}
		}
		cout << '\n';
	}
	cout << "\033[0m";
	preciseDelay(chrono::milliseconds(5000));

	//----------------------------------------LIGHTNING BREADTH FIRST SEARCH----------------------------------------

	priority_queue<pair<ll, ll>, vector<pair<ll, ll>>, CustomCompare> edge;
	vector<vector<pair<ll, ll>>> prev(cols, vector<pair<ll, ll>>(rows, {-1, -1}));
	if(tgtx > cols / 2){
		edge.push({0, 0});
	}else{
		edge.push({cols - 1, 0});
	}
	ll endx, endy;
	while(!edge.empty()){
		pair<ll, ll> curr = edge.top();
		edge.pop();
		ll x = curr.first;
		ll y = curr.second;
		if(visited[x][y]) continue;
		visited[x][y] = true;
		if(x == tgtx && y >= tgty){
			endx = x;
			endy = y;
			break;
		}
		//colour the cell yellow
		lightningAt(y, x);
		//left
		if(x > 0 && !visited[x - 1][y] && ground[x - 1] > y){
			edge.push({x - 1, y});
			prev[x-1][y] = {x, y};
		}
		//right
		if(x < cols - 1 && !visited[x + 1][y] && ground[x + 1] > y){
			edge.push({x + 1, y});
			prev[x+1][y] = {x, y};
		}
		//up
		if(y > 0 && !visited[x][y - 1]){
			edge.push({x, y - 1});
			prev[x][y-1] = {x, y};
		}
		//down
		if(y < rows - 1 && !visited[x][y + 1] && ground[x] > y + 1){
			edge.push({x, y + 1});
			prev[x][y+1] = {x, y};
		}
		preciseDelay(chrono::milliseconds(1000/cols));
	}

	//----------------------------------------TRACE BACK TO START----------------------------------------

	ll x = endx, y = endy;
	ll t = x;
	x = prev[x][y].first;
	y = prev[t][y].second;
	while(x != -1 && y != -1){
		brightLightning(y, x);
		ll temp = x;
		x = prev[x][y].first;
		y = prev[temp][y].second;
		preciseDelay(chrono::milliseconds(400/cols));
	}
	cout << "\033[" << rows - 1 << ";99999999H" << flush;
}
