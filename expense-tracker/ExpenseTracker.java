package main;
import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
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
    

    public void saveToFile(Path filePath) {
        try (BufferedWriter writer = Files.newBufferedWriter(filePath)) {
            writer.write(String.format("# budget=%.2f%n", monthlyBudget));
            for (Expense expense : expenses) {
                writer.write(expense.toCsv());
                writer.newLine();
            }
            System.out.println("Expenses saved to " + filePath);
        } catch (IOException e) {
            System.out.println("Failed to save expenses: " + e.getMessage());
        }
    }
    

    public void loadFromFile(Path filePath) {
        if (!Files.exists(filePath)) {
            return;
        }

        List<Expense> loaded = new ArrayList<>();
        try {
            List<String> lines = Files.readAllLines(filePath);
            for (String line : lines) {
                if (line.isBlank()) {
                    continue;
                }
                if (line.startsWith("# budget=")) {
                    String budgetValue = line.substring("# budget=".length());
                    monthlyBudget = Double.parseDouble(budgetValue);
                    continue;
                }
                loaded.add(Expense.fromCsv(line));
            }
            expenses = new ArrayList<>(loaded);
            System.out.println("Loaded " + expenses.size() + " expenses from " + filePath);
        } catch (IOException | IllegalArgumentException e) {
            System.out.println("Failed to load expenses: " + e.getMessage());
        }
    }
}
