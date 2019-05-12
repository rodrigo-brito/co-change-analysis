from efficient_apriori import apriori

class Apriori:
    def __init__(self, transactions, support=0.5, confidence=0.5, max_length=8):
        self.transactions = transactions
        self.support = support
        self.confidence = confidence
        self.max_length = max_length

    def set_support(self, support):
        self.support = support

    def set_confidence(self, confidence):
        self.confidence = confidence
        
    def set_max_length(self, max_length):
        self.max_length = max_length

    def get_rules(self):
        _, rules = apriori(self.transactions, min_support=self.support, min_confidence=self.confidence, max_length=self.max_length)
        return rules

    def get_rules_csv(self, limit):
        for index, rule in enumerate(self.get_rules()):
            if limit and index > limit:
                break
            print('%s,%s,"%.0f","%.3f"' % ('/'.join(rule.lhs), '/'.join(rule.rhs), rule.support*rule.num_transactions, rule.confidence))
