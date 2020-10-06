
# Goldfish
---

Goldfish is the keyword spotting model used by friday. For instructions on training your own see [preprocessing](preprocessing.md) 
and [model_golding](model_golding.md). 

### Dependencies
Bazel does not work that great with python yet, I recommend starting a virtual env in the goldfish directory

```bash
virtualenv .venv -p python3.6 && . .venv/bin/activate
```

Then installing the requirements

```bash
python3 -m pip3 install -r requirements.txt
```

Hopefully the commands in the other docs should work as expected now.