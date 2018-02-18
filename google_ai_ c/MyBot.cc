#include <iostream>
#include "PlanetWars.h"

FILE  *fPtr;

// The DoTurn function is where your code goes. The PlanetWars object contains
// the state of the game, including information about all planets and fleets
// that currently exist. Inside this function, you issue orders using the
// pw.IssueOrder() function. For example, to send 10 ships from planet 3 to
// planet 8, you would say pw.IssueOrder(3, 8, 10).
//
// There is already a basic strategy in place here. You can use it as a
// starting point, or you can throw it out entirely and replace it with your
// own. Check out the tutorials and articles on the contest website at
// http://www.ai-contest.com/resources.
void DoTurn(PlanetWars& pw, int diff_ships) {

	int num_planets = pw.planets_.size();
	int* ptr_pcm_pos;
	int* ptr_pcm_end;
	int min_move_profit = 99999999;
	int min_planet_to_profit = -1;
	int c;
				
		auto planet_min_cost = [&]()
		{
			if( ( diff_ships > 0 && pw.planets_[c].owner_ != 1 || pw.planets_[c].owner_ == 0 )
				&& pw.planets_[c].growth_rate_ != 0 && pw.planets_[c].pred_numships <= 0 )
						{
							int moves = *ptr_pcm_pos -1 * pw.planets_[c].pred_numships/pw.planets_[c].growth_rate_;
							if( moves < min_move_profit )
							{
								min_move_profit = moves;
								min_planet_to_profit = c;
							}
						}
		};
		
		

		for ( int p = 0; p < num_planets; p++ )
		{
			if( pw.planets_[p].owner_ == 1 )
			{
				min_move_profit = 99999999;
				min_planet_to_profit = -1;
				
				//row base search optermization

					ptr_pcm_pos = pw.planet_cost_matrix + p * num_planets; 
					ptr_pcm_end = ptr_pcm_pos + num_planets; 
					ptr_pcm_pos += p + 1;
			
					for (c = 0 ; ptr_pcm_pos < ptr_pcm_end; ptr_pcm_pos++, c++ )
					{
						planet_min_cost();
					}
				
				//column Base search
					ptr_pcm_pos = pw.planet_cost_matrix + p; 
					ptr_pcm_end = ptr_pcm_pos + num_planets * p; 

					for (c = 0 ; ptr_pcm_pos < ptr_pcm_end; ptr_pcm_pos++, c++ )
					{
						planet_min_cost();
					}


				/*
				//column based search optermizations
				for( int r = p; r < num_planets; r++)
				{
					int* ptr_pcm_end = ptr_pcm_pos + num_planets; 
					ptr_pcm_pos += r + 1;
			
					for (int c = 0 ; ptr_pcm_pos < ptr_pcm_end; ptr_pcm_pos++, c++ )
					{
						if(pw.planets_[c].owner_ != 0)
						{
							int moves = *ptr_pcm_pos + pw.planets_[c].num_ships_/pw.planets_[c].growth_rate_;
							if( moves < min_move_profit )
							{
								min_move_profit = moves;
								min_planet_to_profit = c;
							}
						}
					}
				}
				*/
				if ( min_planet_to_profit != -1 )
				{
					int ships_to_send;
						
					if( -1 * pw.planets_[min_planet_to_profit].pred_numships + 5 > pw.planets_[p].num_ships_ )
						return;
					else
						ships_to_send = -1 * pw.planets_[min_planet_to_profit].pred_numships + 1 ;

					pw.planets_[p].num_ships_ -= ships_to_send;
					pw.planets_[min_planet_to_profit].num_ships_ += ships_to_send;
					pw.planets_[min_planet_to_profit].pred_numships += ships_to_send;
					pw.IssueOrder(p, min_planet_to_profit,ships_to_send);
				}
				

			}

		}
		return;
		

  // (1) If we currently have a fleet in flight, just do nothing.
 /* if (pw.MyFleets().size() >= 1) {
    return;
  }*/
  // (2) Find my strongest planet.
  int source = -1;
  double source_score = -999999.0;
  int source_num_ships = 0;
  std::vector<Planet> my_planets = pw.MyPlanets();
  for (int i = 0; i < my_planets.size(); ++i) {
    const Planet& p = my_planets[i];
    double score = (double)p.NumShips();
    if (score > source_score) {
      source_score = score;
      source = p.PlanetID();
      source_num_ships = p.NumShips();
    }
  }
  // (3) Find the weakest enemy or neutral planet.
  int dest = -1;
  double dest_score = -999999.0;
  std::vector<Planet> not_my_planets = pw.NotMyPlanets();
  for (int i = 0; i < not_my_planets.size(); ++i) {
    const Planet& p = not_my_planets[i];
    double score = 1.0 / (1 + p.NumShips());
    if (score > dest_score) {
      dest_score = score;
      dest = p.PlanetID();
    }
  }
  // (4) Send half the ships from my strongest planet to the weakest
  // planet that I do not own.
  if (source >= 0 && dest >= 0) {
    int num_ships = source_num_ships / 2;
    pw.IssueOrder(source, dest, num_ships);
  }
}

//#define get_char std::cin.get();

#define get_char c = std::cin.get(); putc(c,fPtr);



// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.

void read_token( char* str, const char stop_token )
{
			char c = stop_token;
			char p = -1;
			do
			{
				c =  (char) get_char
				str[++p] =  c;
			} while ( c != stop_token );
			str[p] = '\0';

}

void read_skip_token(const char stop_token, char count)
{
	char c;
	for( char i = 0; i < count ; i++ )
	{
		do
		{
			c = std::cin.get();
			putc(c,fPtr);
			/*
			{
			fclose(fPtr);
			fPtr = fopen("bot_log.txt","a");
			}*/
		}
		while ( c != stop_token && c != '\n'  );
		
		
	}
}


void fast_pass_game()
{
  char token[15];
	bool initialize = true;

	
  fPtr = fopen("bot_log.txt","w");

	PlanetWars pw("");

	double spacial_x_min = 0.0, spacial_y_min = 0.0, spacial_x_max = 0.0, spacial_y_max = 0.0;
  
	int p = 0;
	int f = 0;
	int c = 0;

	int numships;
	int owner;
	int des;

	Planet planet;
	Fleet fleet;
	int enamy_ships = 0;
	int my_ships = 0;
	int diff_ships = 0;

	bool run = true;

	while ( run )
	{

    c = get_char

		switch (c)
		{
			case '\n':
			{
			}break;

			case '#':
			{
				read_skip_token('\n', 2);
			} break;

			case 'P':
			{
				get_char 	// read first blank char

				if ( initialize )
				{
					//Read Planet information all on first path on bit on second pass
					pw.planets_.push_back( planet );
				
					//p = pw.planets_.size() - 1;
				
					read_token( token, ' ' );			
					pw.planets_[p].x_ = atoi(token);

					if( pw.planets_[p].x_ < spacial_x_min )
						spacial_x_min = pw.planets_[p].x_;

					if( pw.planets_[p].x_ > spacial_x_max )
						spacial_x_max = pw.planets_[p].x_;

					read_token( token, ' ' );			
					pw.planets_[p].y_ = atoi(token);

					if( pw.planets_[p].y_ < spacial_y_min )
						spacial_y_min = pw.planets_[p].y_;

					if( pw.planets_[p].y_ > spacial_y_max )
						spacial_y_max = pw.planets_[p].y_;

					read_token( token, ' ' );		
					owner = atoi(token);
					pw.planets_[p].owner_ = owner;

					if ( owner != 1 )
						numships = -numships;
					read_token( token, ' ' );			
					numships = atoi(token);
					pw.planets_[p].num_ships_ = numships;
					pw.planets_[p].pred_numships = numships;

					diff_ships += numships;

					read_token( token, '\n' );			
					pw.planets_[p].growth_rate_ = atoi(token);
				
					pw.planets_[p].planet_id_ = p;
					p++;
				}
				else
				{
					read_skip_token(' ', 2);
					read_token( token, ' ' );
					owner = atoi(token);
					pw.planets_[p].owner_ = owner;
					read_token( token, ' ' );			
					pw.planets_[p].num_ships_ = atoi(token);
					pw.planets_[p].pred_numships = (owner == 1)? numships: -1 * numships;
					read_skip_token('\n', 1);		
					//read_token( fPtr, token, '\n' );	
					p++;
				}
				

			} break;

			case 'F':
			{
				get_char 	// read first blank char

				if ( f >= pw.fleets_.size() )
					pw.fleets_.push_back(fleet);

				read_token( token, ' ' );
				owner = atoi(token);
				pw.fleets_[f].owner_ = owner;

				read_token( token, ' ' );		
				numships = atoi(token);
				pw.fleets_[f].num_ships_ = numships;

				read_token( token, ' ' );		
				pw.fleets_[f].source_planet_ = atoi(token);

				if (owner != 1) 
					numships = -numships;

				read_token( token, ' ' );		
				des = atoi(token);
				pw.fleets_[f].destination_planet_ = des;
				pw.planets_[des].pred_numships += numships;
				diff_ships += numships;

				
				read_token( token, ' ' );		
				pw.fleets_[f].total_trip_length_ = atoi(token);

				read_token( token, '\n' );		
				pw.fleets_[f].turns_remaining_ = atoi(token);
				f++;
			}break;

			case 'g':
			{
				c = get_char
					if ( c == 'o' )
					{
						if ( initialize )
						{
							fprintf(fPtr,"%f\n", spacial_x_min );
							fprintf(fPtr,"%f\n", spacial_x_max );
							fprintf(fPtr,"%f\n", spacial_y_min );
							fprintf(fPtr,"%f\n", spacial_y_max );
							pw.calculate_matrix();
						}

						initialize = false;

						c = get_char
						if( c == '\n')
						{
							DoTurn(pw, diff_ships);
							pw.FinishTurn();
							pw.fleets_.clear();
							p = 0;
							f = 0;
							enamy_ships = 0;
							my_ships = 0;
							diff_ships = 0;
						}
					}
			}break;
			
			default:
				get_char;
				fclose(fPtr);
				//run = false;
		}

	}

	delete pw.planet_cost_matrix;
}



int main(int argc, char *argv[]) {

	fast_pass_game();
	
 /* std::string current_line;
  std::string map_data;
  FILE  *fPtr;
  fPtr = fopen("bot_log.txt","a+");

	
  while (true) {

    int c = std::cin.get();
	putc(c,fPtr);
    current_line += (char)c;
    if (c == '\n') {
      if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
		
        PlanetWars pw(map_data);
        map_data = "";
        DoTurn(pw);
	pw.FinishTurn();
      } else {
        map_data += current_line;
		
	
      }
      current_line = "";
    }
  }

  fclose(fPtr);
		*/

  return 0;
}




/*

void read_token( FILE* fPtr, char* str, const char stop_token )
{
			char c = stop_token;
			char p = -1;
			do
			{
				c = (char) std::cin.get();
				putc(c, fPtr);
				str[++p] =  c;
			} while ( c != stop_token );
			str[p] = '\0';

}

void read_skip_token(const char stop_token, char count)
{
	for( char i = 0; i < count ; i++ )
		while ( std::cin.get() != stop_token );	
}

void fast_pass_game()
{
	std::string current_line;
	std::string map_data;

  char token[100];
  FILE  *fPtr = NULL;
  fPtr = fopen("bot_log.txt","a+");
	bool initialize = true;

	PlanetWars pw("");
  
	int p = 0;
	int f = 0;
	int c = 0;

	Planet planet;
	Fleet fleet;

	while (true)
	{
		if( fPtr == NULL )
			fPtr = fopen("bot_log.txt","a+");

    c = std::cin.get();
		map_data += (char) c;

		switch (c)
		{
			case '\n':
			{
				putc('\n',fPtr);
				putc('!',fPtr);
				putc(c,fPtr);
			}break;

			case '#':
			{
				putc('\n',fPtr);
				putc('#',fPtr);
				do
				{
					c = std::cin.get();
					putc(c,fPtr);
				}
				while ( c != '\n' );

			} break;

			case 'P':
			{
				std::cin.get();
				if ( initialize )
				{
					putc('\n',fPtr);
					putc('P',fPtr);
					//Read Planet information all on first path on bit on second pass
					pw.planets_.push_back( planet );
				
					//p = pw.planets_.size() - 1;
				
					read_token( fPtr, token, ' ' );			
					pw.planets_[p].x_ = atoi(token);

					read_token( fPtr, token, ' ' );			
					pw.planets_[p].y_ = atoi(token);

					read_token( fPtr, token, ' ' );			
					pw.planets_[p].owner_ = atoi(token);

					read_token( fPtr, token, ' ' );			
					pw.planets_[p].num_ships_ = atoi(token);

					read_token( fPtr, token, '\n' );			
					pw.planets_[p].growth_rate_ = atoi(token);
				
					pw.planets_[p].planet_id_ = p;
					p++;
				}
				else
				{
					read_skip_token(' ', 2);
					read_token( fPtr, token, ' ' );		
					pw.planets_[p].owner_ = atoi(token);
					read_token( fPtr, token, ' ' );			
					pw.planets_[p].num_ships_ = atoi(token);
					read_skip_token('\n', 1);		
					//read_token( fPtr, token, '\n' );	
					p++;
				}
				

			} break;

			case 'F':
			{
				std::cin.get();
				putc('\n',fPtr);
				putc('F',fPtr);
				if ( f >= pw.fleets_.size() )
					pw.fleets_.push_back(fleet);

				read_token( fPtr, token, ' ' );		
				pw.fleets_[f].owner_ = atoi(token);

				read_token( fPtr, token, ' ' );		
				pw.fleets_[f].num_ships_ = atoi(token);

				read_token( fPtr, token, ' ' );		
				pw.fleets_[f].source_planet_ = atoi(token);

				read_token( fPtr, token, ' ' );		
				pw.fleets_[f].destination_planet_ = atoi(token);

				read_token( fPtr, token, ' ' );		
				pw.fleets_[f].total_trip_length_ = atoi(token);

				read_token( fPtr, token, '\n' );		
				pw.fleets_[f].turns_remaining_ = atoi(token);
				f++;
			}break;

			case 'g':
			{
				putc('\n',fPtr);
				putc('g',fPtr);

				c = std::cin.get();
					putc(c,fPtr);
					if ( c == 'o' )
					{
						c = std::cin.get();
						putc(c,fPtr);
						if( c == '\n')
						{
						putc('@',fPtr);
						//std::cout << "go" << std::endl;
						//std::cout.flush();
						putc('%',fPtr);

						//PlanetWars pw2(map_data);
						//map_data = "";
						DoTurn(pw);
						pw.FinishTurn();

						pw.fleets_.clear();
						p = 0;
						f = 0;
						
						fclose(fPtr); 
						if( fPtr == NULL )
				fPtr = fopen("bot_log.txt","a+");
					//	pw.FinishTurn();
					
					
						}
					}

				initialize = false;
			}break;

			default:
			{
				putc('\n',fPtr);
				putc('$',fPtr);
				putc(c,fPtr);
			}break;
		}

	}

	   fclose(fPtr);  
	
  
  
  
}



*/