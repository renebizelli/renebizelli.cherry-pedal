from dataclasses import dataclass


@dataclass(frozen=True)
class Band:
    id: str
    name: str
    logo: bool
