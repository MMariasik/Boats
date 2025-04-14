#ifndef BUTTONFUNCTIONS
#define BUTTONFUNCTIONS

namespace fnc
{
	namespace MainMenu
	{
		void joinGame();
		void hostGame();
		void settings();
		void how2Play();

		void deleteGetCodeGUI();
		void copyCodeToClipBoard();
		void enterInvitingCode();

	}
	namespace Config
	{
		void imReady();
	}
	namespace inGame
	{
		void shoot();
		void surrender();
		void backToMenu();
	}
}

#endif // !BUTTONFUNCTIONS
