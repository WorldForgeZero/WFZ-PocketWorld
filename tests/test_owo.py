import pytest


@pytest.fixture(autouse=True)
def manage_owo():
    yield


def test_owo():
    print("owo")
