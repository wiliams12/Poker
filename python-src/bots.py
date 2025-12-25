import random
import lib

import matplotlib.pyplot as plt
import numpy as np
from typing import List, Dict, Any

moves = [
    "all in\n",
    "fold\n",
    "call\n",
    "raise\n",
    "check\n"
]

class CMDError(Exception):
    def __init__(self, message, custom_data=None):
        super().__init__(message)

class Bot():
    def __init__(self, process):
        self.wins = 0
        self.process = process
        self.name = "bot"

    # returns an index of a move 0-4
    def choose_move(self):
        pass

    # returns a valid amount to be raised
    def raise_amount(self, budget):
        pass

    def play(self):
        move = moves[self.choose_move()]
        budget = 0
        if move == "raise\n":
            budget = lib.get_bank(self.process)
        lib.write_to_stdin(self.process, move)
        response = lib.read_from_stdout(self.process)
        if move == "raise\n" and response != "invalid":
            #response != "invalid":
            if response == "successfully played":
                raise CMDError
            while True:
                amount = self.raise_amount(budget)
                lib.write_to_stdin(self.process, str(amount) + "\n")
                response = lib.read_from_stdout(self.process)
                if response == "amount raised":
                    response = lib.read_from_stdout(self.process)
                    if response == "successfully played":
                        break
                    else:
                        raise CMDError

        return response

class BobTheRandom(Bot):
    def __init__(self, process):
        super().__init__(process)
        self.name = "Bob the Random"

    def choose_move(self):
        return random.randint(0,4)

    def raise_amount(self, budget):
        return random.randint(1,budget)
    
class AlvinTheBold(Bot):
    def __init__(self, process):
        super().__init__(process)
        self.name = "Alvin the Bold"
    
    def choose_move(self):
        return 0

    def raise_amount(self, budget):
        print("Logic error: Alvin should never be able to play raise!")
        return budget

class Record():
    def __init__(self, player_num):
        self.player_num = player_num
        self.data = {
            "banks" : [],
            "wins" : []
        }

    def store_data(self, banks, wins):
        self.data["banks"] = banks
        self.data["wins"] = wins



# TODO:
# track bank and track wins

class Game():
    def __init__(self, process, bots):
        self.process = process
        self.bots = bots
        self.num_of_players = len(self.bots)
        self.history = []

    def simulate(self):
        record = Record(self.num_of_players)
        banks = []
        wins = []
        for i in range(self.num_of_players):
            banks.append(2400)
            wins.append(0)
        record.store_data(banks, wins)
        self.history.append(record)
        while True:
            # wait for make a move
            # play a move with appropraite bot
            # redo if not successful
            # check for round ended
            if self.process.poll() is not None:
                print(f"C Program ended with code: {self.process.returncode}")
                break

            response = lib.read_from_stdout(self.process)
            if response == "move:":
                to_move = int(lib.read_from_stdout(self.process))
                while True:
                    response = self.bots[to_move].play()
                    if response == "successfully played":
                        break

            elif response == "round ended":
                print("ended")
                results = lib.read_from_stdout(self.process)
                winners = results.split("~")[0].split(" ")
                banks = results.split("~")[1].split(" ")

                for index in winners:
                    if index.isdigit():
                        self.bots[int(index)].wins += 1
  
                wins = []
                for bot in self.bots:
                    wins.append(bot.wins)


                record = Record(self.num_of_players)
                record.store_data(banks, wins)
                self.history.append(record)

            else:
                print(f"not ended. moves played: {len(self.history)}")


    def plot_history(self, title: str = "Poker Bot Performance History"):
        """
        Transposes the turn-based data in self.history into player-based time series 
        and generates two subplots for 'banks' and 'wins'.
        """
        if not self.history:
            print("History is empty. Cannot plot.")
            return

        # 1. Transpose Data (Turn-based -> Player-based)
        # Initialize lists where the outer index is the player ID
        player_bank_history = [[] for _ in range(self.num_of_players)]
        player_win_history = [[] for _ in range(self.num_of_players)]

        # Iterate through the history (turns)
        for record in self.history:
            for player_idx in range(self.num_of_players):
                # --- FIX: FORCE INTEGER CONVERSION HERE ---
                # This ensures Matplotlib treats values as numbers, not text labels
                try:
                    bank_val = int(record.data["banks"][player_idx])
                    win_val = int(record.data["wins"][player_idx])
                except (ValueError, TypeError):
                    # Safety fallback if data is corrupt
                    bank_val = 0
                    win_val = 0
                    
                player_bank_history[player_idx].append(bank_val)
                player_win_history[player_idx].append(win_val)

        # 2. Setup Plotting Variables
        num_turns = len(self.history)
        turns = np.arange(1, num_turns + 1)
        # Starts counting from 0 by default
        player_labels = {i: f"Player: {bot.name}" for i, bot in enumerate(self.bots)}
        
        fig, axes = plt.subplots(nrows=2, ncols=1, figsize=(10, 8), sharex=True)
        fig.suptitle(title, fontsize=16)

        # --- PLOT 1: CHIPS HISTORY (BANKS) ---
        ax_chips = axes[0]
        for i in range(self.num_of_players):
            ax_chips.plot(
                turns, 
                player_bank_history[i], 
                label=player_labels[i], 
                marker='.', 
                linestyle='-'
            )
        ax_chips.set_title('Chips History (Bank)')
        ax_chips.set_ylabel('Chips')
        ax_chips.grid(True, linestyle='--', alpha=0.6)
        ax_chips.legend(loc='upper left')
        
        # Optional: Force Y-axis to start at 0 if you want to ground the graph
        # ax_chips.set_ylim(bottom=0) 

        # --- PLOT 2: WINS HISTORY ---
        ax_wins = axes[1]
        for i in range(self.num_of_players):
            ax_wins.plot(
                turns, 
                player_win_history[i], 
                label=player_labels[i], 
                marker='o', 
                linestyle='--'
            )
        ax_wins.set_title('Wins History (Cumulative)')
        ax_wins.set_xlabel('Turn/Entry Number')
        ax_wins.set_ylabel('Total Wins')
        ax_wins.grid(True, linestyle='--', alpha=0.6)
        ax_wins.legend(loc='upper left')
        
        # Wins should practically always start at 0
        ax_wins.set_ylim(bottom=0) 
        # Force integer ticks on Y-axis for wins (you can't have 1.5 wins)
        ax_wins.yaxis.set_major_locator(plt.MaxNLocator(integer=True))

        plt.tight_layout(rect=[0, 0, 1, 0.96])
        plt.savefig('poker_bot_performance_plot_final.png')
        plt.close(fig)
        print("Plot saved as 'poker_bot_performance_plot_final.png'")