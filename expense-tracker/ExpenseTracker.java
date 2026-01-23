package main;
import java.time.YearMonth;
import java.util.ArrayList;
import java.util.List;

class ExpenseTracker {
    private ArrayList<Expense> expenses;
    private double monthlyBudget;

    
    public ExpenseTracker() {
        expenses = new ArrayList<>();
        monthlyBudget = 0;
    }
    

    public void addExpense(Expense expense) {
        expenses.add(expense);
    }
    

    public void removeExpense(int index) {
        if (index >= 0 && index < expenses.size()) {
            expenses.remove(index);
            System.out.println("Expense removed successfully.");
        } else {
            System.out.println("Invalid expense index.");
        }
    }
    

    public void viewExpenses() {
        if (expenses.isEmpty()) {
            System.out.println("No expenses found.");
        } else {
            System.out.println("Expense List:");
            for (int i = 0; i < expenses.size(); i++) {
                Expense expense = expenses.get(i);
                System.out.printf("%d. Date: %s, Description: %s, Amount: $%.2f%n",
                        i + 1,
                        expense.getDateString(),
                        expense.getDescription(),
                        expense.getAmount());
            }
        }
    }
    

    public void totalExpenses() {
        double total = 0;
        for (Expense expense : expenses) {
            total += expense.getAmount();
        }
        System.out.printf("Total Expenses: $%.2f%n", total);
    }
    

    public double totalForMonth(YearMonth month) {
        double total = 0;
        for (Expense expense : expenses) {
            YearMonth expenseMonth = YearMonth.from(expense.getDate());
            if (expenseMonth.equals(month)) {
                total += expense.getAmount();
            }
        }
        return total;
    }
    

    public List<Expense> searchExpenses(String keyword) {
        List<Expense> matches = new ArrayList<>();
        String needle = keyword.toLowerCase();
        for (Expense expense : expenses) {
            if (expense.getDescription().toLowerCase().contains(needle)
                    || expense.getDateString().contains(needle)) {
                matches.add(expense);
            }
        }
        return matches;
    }
    

    public void setMonthlyBudget(double budget) {
        monthlyBudget = budget;
    }

    
    public double getMonthlyBudget() {
        return monthlyBudget;
    }
    

}
