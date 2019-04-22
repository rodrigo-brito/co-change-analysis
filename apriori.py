from efficient_apriori import apriori


class Apriori:
    def __init__(self, transactions, support=0.5, confidence=1):
        self.transactions = transactions
        self.support = support
        self.confidence = confidence

    def set_support(self, support):
        self.support = support

    def set_confidence(self, confidence):
        self.confidence = confidence

    def get_rules(self):
        _, rules = apriori(self.transactions, min_support=self.support, min_confidence=self.confidence)
        return rules
