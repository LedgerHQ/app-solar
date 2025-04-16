from typing import List, Union

from application_client.solar_transaction import Transaction, TransactionError
from application_client.solar_utils import UINT16_MAX


class Vote(Transaction):
    def __init__(
        self,
        nonce: int,
        senderPkey: Union[str, bytes],
        fee: int,
        memo: str,
        votes: List[List[Union[int, str]]],
        network: int = 63,
        version: int = 3,
        startingByte: int = 0xFF,
    ) -> None:
        super().__init__(
            2, 2, nonce, senderPkey, fee, memo, network, version, startingByte
        )
        self.votes = []
        total_percentage = 0

        for vote in votes:
            username, percentage = vote

            if not isinstance(username, str):
                raise TransactionError(f"Username must be a string, got: {type(username).__name__}")

            if not isinstance(percentage, int):
                raise TransactionError(f"Percentage must be an integer, got: {type(percentage).__name__}")

            if len(username) < 1 or len(username) > 20:
                raise TransactionError(f"Bad username: '{username}'!")

            if not (0 <= percentage <= UINT16_MAX):
                raise TransactionError(f"Bad percentage: '{percentage}'!")

            total_percentage += percentage
            self.votes.append([username, percentage])

        if len(self.votes) > 53:
            raise TransactionError(f"Wrong total number of votes: '{len(self.votes)}'!")

    def serialise(self) -> bytes:
        asset = b""
        for vote in self.votes:
            username, percentage = vote

            if not isinstance(username, str) or not isinstance(percentage, int):
                raise ValueError("Invalid vote format during serialization")

            asset += len(username).to_bytes(1, byteorder="little")
            asset += username.encode("ascii")
            asset += percentage.to_bytes(2, byteorder="little")
        return super().serialise() + b"".join(
            [len(self.votes).to_bytes(1, byteorder="little"), asset]
        )
