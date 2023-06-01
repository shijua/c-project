""" 
Data definitions for the emulator output state
"""
from dataclasses import dataclass
from enum import Enum
from io import StringIO
from pathlib import Path
from typing import Dict, Tuple

###################################################################################################
""" PState """


@dataclass
class PState:
    """Representation of an ARM64 CPU's PState flags"""

    N: bool = False
    Z: bool = False
    C: bool = False
    V: bool = False

    def __str__(self) -> str:
        printVal = lambda name: name if self.__dict__[name] else "-"
        return f"{printVal('N')}{printVal('Z')}{printVal('C')}{printVal('V')}"


def parse_PState(pstate: str) -> PState:
    return PState(*tuple([x != "-" for x in pstate]))


###################################################################################################
""" ARM64 Register Setup """

NUM_REGS = 32
_reg_names = [f"X{i:02}" for i in range(31)] + ["PC"]  # , "SP"]
Reg: Enum = Enum("Reg", {name: i for i, name in enumerate(_reg_names)})


def is_reg_name(name: str) -> bool:
    return name in _reg_names


def str_to_reg(reg: str) -> Reg:
    return eval(f"Reg.{reg}")


###################################################################################################
""" Dict Utils """


def _compare_dicts(
    res: Dict, exp: Dict[str, int], act: Dict[str, int]
) -> Tuple[bool, Dict[str, Dict[str, int]]]:
    """Compares two dictionaries and returns their difference in res"""

    for k in exp.keys() | act.keys():
        if k not in act:
            res[k] = {"exp": exp[k], "act": 0}
        elif k not in exp:
            res[k] = {"exp": 0, "act": act[k]}
        elif k in act and exp[k] != act[k]:
            res[k] = {"exp": exp[k], "act": act[k]}
    return len(res) == 0, res


def compare_dicts(
    exp: Dict[str, int], act: Dict[str, int]
) -> Tuple[bool, Dict[str, Dict[str, int]]]:
    """
    Compares two dictionaries and returns their difference, if any.

    Returns a tuple of (True, {}) if the dictionaries are equal,
    otherwise return (False, diff) where diff is a dictionary of the differences.
    """
    _compare_dicts({}, exp, act)


###################################################################################################
""" CPU State """


@dataclass
class CPU_State:
    """Final CPU State"""

    regs: Dict[Reg, int]
    pstate: PState
    nz_mem: Dict[int, int]

    def dump(self, file: Path):
        """Write the CPU state to a file"""
        with open(file, "w") as f:
            f.write("Registers:\n")
            for reg in Reg:
                f.write(f"{reg.name:<7}= {self.regs[reg]:016x}\n")
            f.write(f"PSTATE : {self.pstate}\n")
            f.write(f"Non-Zero Memory:\n")
            for addr in sorted(self.nz_mem.keys()):
                f.write(f"0x{addr:08x} : {self.nz_mem[addr]:08x}\n")

    def __str__(self) -> str:
        sout = StringIO()
        self.dump(sout)
        return sout.getvalue()

    def compare(self, act: "CPU_State") -> Tuple[bool, Dict[str, Dict[str, int]]]:
        """
        Compares two CPU states and returns their difference, if any.

        Returns a tuple of (True, {}) if the states are equal,
        otherwise returns (False, diff) where diff is a dictionary of the differences.
        """
        difference: Dict[str, Dict[str, int]] = {"regs": {}, "nz_mem": {}}
        _compare_dicts(difference["regs"], self.regs, act.regs)
        _compare_dicts(difference["nz_mem"], self.nz_mem, act.nz_mem)
        if self.pstate != act.pstate:
            difference["pstate"] = {"exp": self.pstate, "act": act.pstate}

        any_diff = (
            len(difference["regs"]) > 0
            or len(difference["nz_mem"]) > 0
            or "pstate" in difference
        )
        return not any_diff, difference


def parse_out_file(actState: Path) -> CPU_State:
    """Parses the custom `.out` file to a CPU_State object"""
    with open(actState, "r") as f:
        lines = f.readlines()
        if len(lines) < 33:
            return None

        # Parse registers
        regs = {}
        for line in lines[1:33]:
            reg, x = [tok.strip() for tok in line.split("=")]
            regs[str_to_reg(reg)] = int(x, 16)

        # Parse PState
        pstate = parse_PState(lines[33].split(":")[1].strip())

        # Parse non-zero memory, if any
        nzmem = {}
        for line in lines[35:]:
            addr, val = [tok.strip() for tok in line.split(":")]
            nzmem[int(addr, 16)] = int(val, 16)

        return CPU_State(regs, pstate, nzmem)
