def write_to_stdin(process, msg):
    process.stdin.write(msg)
    process.stdin.flush()
    print(f"stdin: {msg}")

# Strips any whitespace or newlines !!!
def read_from_stdout(process):
    line = process.stdout.readline()
    if not line:
        return "" # EOF or closed pipe
    res = line.strip()
    print(f"stdout: {res}")
    return res


# ! ValueError if mismatch in communication (due to mapping to int)
def get_bank(process):
    write_to_stdin(process, "2\n")
    response = read_from_stdout(process)
    if response == "" or response == "invalid":
        return 0
    return int(response)

def get_bet(process):
    write_to_stdin(process, "3\n")
    response = read_from_stdout(process)
    if response == "" or response == "invalid":
        return 0
    return int(response)

def get_cards(process):
    write_to_stdin(process, "1\n")
    hand = []
    response = None
    while True:
        response = read_from_stdout(process)
        if (response != "invalid" or response != "end"):
            break
        hand.append(response)

    return hand