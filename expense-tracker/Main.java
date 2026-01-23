package main;
import java.nio.file.Path;
import java.time.LocalDate;
import java.time.YearMonth;
import java.util.List;
import java.util.Scanner;

public class Main {
    private static final Path DATA_FILE = Path.of("expenses.csv");

    public static void main(String[] args) {
        ExpenseTracker tracker = new ExpenseTracker();
        Scanner scanner = new Scanner(System.in);
        tracker.loadFromFile(DATA_FILE);

        while (true) {
            System.out.println("\nExpense Tracker Menu:");
            System.out.println("1. Add Expense");
            System.out.println("2. Remove Expense");
            System.out.println("3. View Expenses");
            System.out.println("4. Total Expenses");
            System.out.println("5. Monthly Summary");
            System.out.println("6. Search Expenses");
            System.out.println("7. Set Monthly Budget");
            System.out.println("8. Save Expenses");
            System.out.println("9. Exit");
            System.out.print("Enter your choice (1-9): ");

            int choice = readInt(scanner);
            scanner.nextLine();

            switch (choice) {
                case 1:
                    LocalDate date = readDate(scanner);
                    System.out.print("Enter the description: ");
                    String description = scanner.nextLine();
                    System.out.print("Enter the amount: ");
                    double amount = readDouble(scanner);
                    Expense expense = new Expense(date, description, amount);
                    tracker.addExpense(expense);
                    System.out.println("Expense added successfully.");
                    break;

                case 2:
                    System.out.print("Enter the expense index to remove: ");
                    int index = readInt(scanner) - 1;
                    tracker.removeExpense(index);
                    break;

                case 3:
                    tracker.viewExpenses();
                    break;

                case 4:
                    tracker.totalExpenses();
                    break;

                case 5:
                    YearMonth month = readMonth(scanner);
                    double total = tracker.totalForMonth(month);
                    System.out.printf("Total for %s: $%.2f%n", month, total);
                    if (tracker.getMonthlyBudget() > 0) {
                        double remaining = tracker.getMonthlyBudget() - total;
                        System.out.printf("Budget: $%.2f, Remaining: $%.2f%n",
                                tracker.getMonthlyBudget(),
                                remaining);
                    }
                    break;

                case 6:
                    System.out.print("Enter a keyword (date or description): ");
                    String keyword = scanner.nextLine();
                    List<Expense> matches = tracker.searchExpenses(keyword);
                    if (matches.isEmpty()) {
                        System.out.println("No matching expenses found.");
                    } else {
                        System.out.println("Matching Expenses:");
                        for (Expense match : matches) {
                            System.out.printf("Date: %s, Description: %s, Amount: $%.2f%n",
                                    match.getDateString(),
                                    match.getDescription(),
                                    match.getAmount());
                        }
                    }
                    break;

                case 7:
                    System.out.print("Enter monthly budget amount: ");
                    double budget = readDouble(scanner);
                    tracker.setMonthlyBudget(budget);
                    System.out.println("Monthly budget set.");
                    break;

                case 8:
                    tracker.saveToFile(DATA_FILE);
                    break;

                case 9:
                    tracker.saveToFile(DATA_FILE);
                    System.out.println("Have a nice day!");
                    scanner.close();
                    return;

                default:
                    System.out.println("Invalid choice, please try again.");
            }
        }
    }

    
    private static int readInt(Scanner scanner) {
        while (!scanner.hasNextInt()) {
            System.out.print("Please enter a valid number: ");
            scanner.next();
        }
        return scanner.nextInt();
    }

    
    private static double readDouble(Scanner scanner) {
        while (!scanner.hasNextDouble()) {
            System.out.print("Please enter a valid amount: ");
            scanner.next();
        }
        return scanner.nextDouble();
    }
    

    private static LocalDate readDate(Scanner scanner) {
        while (true) {
            System.out.print("Enter the date (YYYY-MM-DD): ");
            String dateInput = scanner.nextLine();
            try {
                return LocalDate.parse(dateInput);
            } catch (Exception e) {
                System.out.println("Invalid date format. Try again.");
            }
        }
    }

    
    private static YearMonth readMonth(Scanner scanner) {
        while (true) {
            System.out.print("Enter month (YYYY-MM): ");
            String monthInput = scanner.nextLine();
            try {
                return YearMonth.parse(monthInput);
            } catch (Exception e) {
                System.out.println("Invalid month format. Try again.");
            }
        }
    }
}
