from dataclasses import dataclass


@dataclass(frozen=True)
class Audio:
    id: str
    name: str
    file: str
