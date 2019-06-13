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

    def get_rules_csv(self):
        rules_csv = []
        for index, rule in enumerate(self.get_rules()):
            lhs = rule.lhs[0].split("|")
            rhs = rule.rhs[0].split("|")
            rules_csv.append({'lhs_file': lhs[0], 'rhs_file': rhs[0], 'lhs_function': lhs[1], 'rhs_function': rhs[1], 'support': rule.support*rule.num_transactions, 'confidence': rule.confidence})
        return rules_csv
