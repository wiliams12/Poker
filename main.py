import subprocess
import json
import bots
import lib

# ! needs to flush the prints in C !!! fflush(stdout);

def run_game():
    # Launch the C game
    process = subprocess.Popen(
        ['./main'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,  # Handle data as strings, not bytes
        bufsize=1   # Line buffered
    )

    bot1 = bots.BobTheRandom(process)
    bot2 = bots.BobTheRandom(process)
    bot3 = bots.BobTheRandom(process)
    bot4 = bots.BobTheRandom(process)

    bot_list = [bot1, bot2, bot3, bot4]

    game = bots.Game(process, bot_list)

    game.simulate()

    game.plot_history()


if __name__ == "__main__":
    run_game()
