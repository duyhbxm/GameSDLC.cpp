#include "Common_Function.h"
#include "MainObject.h"
#include "ThreatsObject.h"
#include "ExplosionObject.h"

bool Init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return false;

	}
	g_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if (g_screen == NULL)
		return false;

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
		return false;

	//Read file way audio
	g_sound_bullet[0] = Mix_LoadWAV("Laser.wav");
	g_sound_bullet[1] = Mix_LoadWAV("Fire1.wav");
	g_sound_exp[0] = Mix_LoadWAV("Explosion.wav");
	if (g_sound_exp[0] == NULL || g_sound_bullet[0] == NULL || g_sound_bullet[1] == NULL)
	{
		return false;
	}


	return true;
}



int main(int arc, char* argv[])
{
	bool is_run_screen = true;
	int bkgn_x = 0;
	bool is_quit = false;
	if (Init() == false) {
		return 0;
	}
	g_bkground = SDLCommonFunc::LoadImage("bg4800.png");
	if (g_bkground == NULL) {
		return 0;
	}


	MainObject plane_object;
	plane_object.SetRect(100, 200);
	bool ret = plane_object.LoadImg("plane_fly.png");
	if (!ret)
	{
		return 0;
	}

	ExplosionObject exp_main;
	ret = exp_main.LoadImg("exp_main.png");
	exp_main.set_clip();
	if (ret == false ) return 0;

	ThreatObject* p_threats = new ThreatObject[NUM_THREATS];
	for (int t = 0; t < NUM_THREATS; t++)
	{
		ThreatObject* p_threat = (p_threats + t);
		if (p_threats)
		{
			ret = p_threat->LoadImg("af1.png");
			if (ret == false)
			{
				return 0;
			}


			int rand_y = rand() % 400;
			if (rand_y > SCREEN_HEIGHT - 200)
			{
				rand_y = SCREEN_HEIGHT * 0.3;
			}


			p_threat->SetRect(SCREEN_WIDTH + t * 400, rand_y);
			p_threat->set_x_val(3);

			AmoObject* p_amo = new AmoObject();
			p_threat->InitAmo(p_amo);
		}
	}

	while (!is_quit) {
		while (SDL_PollEvent(&g_even))
		{
			if (g_even.type == SDL_QUIT) {
				is_quit = true;
				break;
			}
			plane_object.HandleInputAction(g_even, g_sound_bullet);
		}

		//bkgn_x -= 2;


		//SDLCommonFunc::ApplySurface(g_bkground, g_screen, bkgn_x, 0);
		//SDLCommonFunc::ApplySurface(g_bkground, g_screen, bkgn_x + SCREEN_WIDTH, 0);
		//if (bkgn_x <= -SCREEN_WIDTH)
		//{
		//	bkgn_x = 0;
		//}
		if (is_run_screen == true)
		{
			bkgn_x -= 2;
			if (bkgn_x <= -(WIDTH_BACKGROUND - SCREEN_WIDTH))
			{
				is_run_screen = false;
			}
			else
			{
				SDLCommonFunc::ApplySurface(g_bkground, g_screen, bkgn_x, 0);
			}
		}
		else
		{
			SDLCommonFunc::ApplySurface(g_bkground, g_screen, bkgn_x, 0);
		}
		plane_object.HandleMove();
		plane_object.Show(g_screen);
		plane_object.MakeAmo(g_screen);
		for (int tt = 0; tt < NUM_THREATS; tt++)
		{
			ThreatObject* p_threat = (p_threats + tt);
			if (p_threats)
			{
				p_threat->HandleMove(SCREEN_WIDTH, SCREEN_HEIGHT);
				p_threat->Show(g_screen);
				p_threat->MakeAmo(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);



				//COLLISION THREAT BULLET -> MAIN OBJECT
				bool is_col1 = false;
				std::vector<AmoObject*> amo_arr = p_threat->GetAmoList();
				for (int am = 0; am < amo_arr.size(); am++)
				{
					AmoObject* p_amo = amo_arr.at(am);
					if (p_amo)
					{
						is_col1 = SDLCommonFunc::CheckCollisison(p_amo->GetRect(), plane_object.GetRect());
						if (is_col1 == true)
						{
							p_threat->ResetAmo(p_amo);
							break;
						}
					}
				}

				

				bool is_col = SDLCommonFunc::CheckCollisison(plane_object.GetRect(), p_threat->GetRect());
				if (is_col1 || is_col)
				{
					for (int ex = 0; ex < 4; ex++)
					{
						int x_pos = (plane_object.GetRect().x + plane_object.GetRect().w * 0.5) - EXP_WIDTH * 0.5;
						int y_Pos = (plane_object.GetRect().y + plane_object.GetRect().h * 0.5) - EXP_HEIGHT * 0.5;
						exp_main.set_frame(ex);
						exp_main.SetRect(x_pos, y_Pos);
						exp_main.ShowEx(g_screen);
						SDL_Delay(500);
						if (SDL_Flip(g_screen) == -1)
							return 0;
					}

					//add them file no cho may bay minh

					if (MessageBox(NULL, L"GAME OVER", L"Info", MB_OK) == IDOK)
					{
						delete[] p_threat;
						SDLCommonFunc::ClearUp;
						SDL_Quit();
						return 1;
					}
				}
				std::vector<AmoObject*> amo_list = plane_object.GetAmoList();
				for (int im = 0; im < amo_list.size(); im++)
				{
					AmoObject* p_amo = amo_list.at(im);
					if (p_amo != NULL)
					{
						bool ret_col = SDLCommonFunc::CheckCollisison(p_amo->GetRect(), p_threat->GetRect());
						if (ret_col)
						{

							for (int tx = 0; tx < 4; tx++)
							{
								int x_pos = p_amo->GetRect().x - EXP_WIDTH * 0.5;
								int y_Pos = p_amo->GetRect().y - EXP_HEIGHT * 0.5;
								exp_main.set_frame(tx);
								exp_main.SetRect(x_pos, y_Pos);
								exp_main.ShowEx(g_screen);
								if (SDL_Flip(g_screen) == -1)
									return 0;
							}
							p_threat->Reset(SCREEN_WIDTH + tt*400);
							plane_object.RemoveAmo(im);
							Mix_PlayChannel(-1, g_sound_exp[0], 0);
						}
					}
				}
			}
		}
		if (SDL_Flip(g_screen) == -1)
			return 0;
	}
	delete[] p_threats;
	SDLCommonFunc::ClearUp();
	SDL_Quit();
	return 0;
}