#include "controler/WHR/proba_win.h"
#include <cmath>

void ProbaWin::running_algorithm(int round) {
	running = true;	 
	std::cout << "\n\n\nProba_win.cpp - Starting WHR Algorithm" << std::endl;

	while(running && round != 0) {
		update_pool_players();

		for (auto p: ps) {
			std::cout << "Proba_win.cpp - Doing player " << p.id << std::endl;
			naive_max_log_implementation(p);
		}
		round--;
	}	 

	for (auto g : gs) {
		std::cout << "Level Player 1 : " << g.getLevelPlayer1() << ", Level Player 2 : " << g.getLevelPlayer2() << std::endl;  
	}
}

void ProbaWin::update_pool_players() {
	GameDB gdb(msql);
	GameInfoDB gidb(msql);
	PlayerDB pdb(msql);

	gs.clear();
	gs = gdb.read_games();
	std::cout << "Proba_win.cpp - Games in the pool : " << gs.size() << std::endl;
	
	std::vector<Player> tmpps = pdb.read_players();
	ps.clear();
	for (auto p: tmpps) {
		PlayerLinkedToGame tmp(p.getId());

		for (int i = 0; i < gs.size(); ++i) {
			std::vector<GameInformation> gis = gidb.read_gameinfos_from_game_id(gs[i].getId());
			std::cout << "Proba_win.cpp - We found " << gis.size() << " informations for the game " << gs[i].getId() << std::endl;

			for (auto gi: gis) { 
				std::cout << "Proba_win.cpp - Information type : " << gi.getType() << std::endl;

				if (gi.getType() == 0) {
					std::cout << "Proba_win.cpp - This game has a winner" << std::endl;

					int winner = std::stoi(gi.getValue());	
						std::cout << "Proba_win.cpp - Comparing " << gs[i].getIdPlayer1() << "/" << p.getId() << std::endl;

					if(gs[i].getIdPlayer1() == p.getId()) {
						std::cout << "Proba_win.cpp - Game added" << std::endl;

						tmp.gs.push_back(&gs[i]);
						if (winner == 1)
							tmp.f.push_back(true);
						else
							tmp.f.push_back(false);
					} 
					
					std::cout << "Proba_win.cpp - Comparing " << gs[i].getIdPlayer2() << "/" << p.getId() << std::endl;

					if(gs[i].getIdPlayer2() == p.getId()) {
						std::cout << "Proba_win.cpp - Game added" << std::endl;

						tmp.gs.push_back(&gs[i]);
						if (winner == 1)
							tmp.f.push_back(false);
						else
							tmp.f.push_back(true);
					}
					break; 
				}
			}
		}
		ps.push_back(tmp);
	}
	std::cout << "Proba_win.cpp - Players in the pool : " << ps.size() << std::endl;
}

void ProbaWin::naive_max_log_implementation(PlayerLinkedToGame & p) {
	LS l = likelihood(p);
	double lh = l.gl;
	double tmp_lh = lh - 10;

	int i = 0;
	if (p.gs.size() > 0)
	while (i < MAXIMUM_ITER) {
		std::cout << "Proba_win.cpp - Current likelihood : " << lh << std::endl;
		tmp_lh = lh;
		for (int i = 1; i < p.gs.size(); ++i) {
			if (p.id == p.gs[i]->getIdPlayer1()) {
				float nl = p.gs[i]->getLevelPlayer1() + l.d[i]*STEP;
				std::cout << "Proba_win.cpp - New Level of the player 1 " << nl << ", for derivative " << l.d[i-1] << std::endl;
				p.gs[i]->setLevelPlayer1(nl);
			}
			else {
				float nl = p.gs[i]->getLevelPlayer2() + l.d[i]*STEP;
				std::cout << "Proba_win.cpp - New Level of the player 2 " << nl << ", for derivative " << l.d[i-1] << std::endl;
				p.gs[i]->setLevelPlayer2(nl);
			}
		}
		i++;
		l = likelihood(p);
		double lh = l.gl;
	}
}

LS ProbaWin::likelihood(PlayerLinkedToGame & p) {
	LS r;
	for (int i=1; i < p.gs.size(); ++i) {
		DD d; 
		if (p.id == p.gs[i]->getIdPlayer1())
			d = compute_likehood_situation(	p.gs[i]->getLevelPlayer1(), p.gs[i]->getDateGame(), p.gs[i-1]->getLevelPlayer1(), p.gs[i-1]->getDateGame(),
												p.gs[i]->getLevelPlayer2(), p.f[i]);
		else
			d = compute_likehood_situation(	p.gs[i]->getLevelPlayer2(), p.gs[i]->getDateGame(), p.gs[i-1]->getLevelPlayer2(), p.gs[i-1]->getDateGame(),
												p.gs[i]->getLevelPlayer2(), p.f[i]);

		r.gl += d.value;
		r.d.push_back(d.derivative);
	}
	return r;
} 

DD ProbaWin::compute_likehood_situation(float r1, int t1, float r2, int t2, float ro, bool r1_winner) {
	DD dd1 = compute_log_proba_change(r1, t1, r2, t2);
	DD dd2;
	if (r1_winner) 
		dd2 = compute_log_proba_victory(r1, ro);
	else
		dd2 = compute_log_proba_victory(ro, r1);
	DD ret;
	ret.derivative = dd2.derivative + dd1.derivative;
	ret.value = dd2.value + dd1.value;
	return ret; 
}

DD ProbaWin::compute_log_proba_change(float l1, int t1, float l2, int t2) {
	DD ret;
	ret.derivative = (l1-l2)/OMEGA;
	ret.value = std::log10(1/(std::sqrt(abs(t2-t1))*OMEGA*STPI)) - ((l1-l2)*(l1-l2)/(2*(t2-t1)*OMEGA));

	return ret;
}

DD ProbaWin::compute_log_proba_victory (float l1, float l2) {
	DD ret;
	ret.derivative = 1/l1 - 1/(l1+l2);
	ret.value = log10(compute_proba_victory(l1, l2));
	return ret;
}

double ProbaWin::compute_proba_victory(float l1, float l2) {
	double p = 1 / (1+pow(10, 0 - (l1 - l2) / ELO_CONSTANTE));
	std::cout << "Proba_win.cpp - Proba victory computed : " << p << std::endl; 
	return p;
}